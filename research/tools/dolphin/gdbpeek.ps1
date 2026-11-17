# gdbpeek.ps1: attach to Dolphin's GDB stub on localhost, let the game run until a line matching
# -Pattern appears in -Log, then stop the CPU and print the registers and the stack's back chain.
# Runs on the Windows side because the firewall blocks WSL from the stub's port: run it from
# PowerShell, e.g. via powershell.exe -File from WSL (research/notes/pure-link.md).
param([int]$Port = 2345, [string]$Log, [string]$Pattern, [int]$Timeout = 600, [string]$Out, [string]$Watch = "", [int]$MaxHits = 40)
$ErrorActionPreference = "Stop"
trap { Add-Content -Path $Out -Value ("ERROR: " + $_ + " at " + $_.InvocationInfo.PositionMessage); break }
function Csum([string]$s) { $c = 0; foreach ($ch in $s.ToCharArray()) { $c = ($c + [int]$ch) % 256 }; "{0:x2}" -f $c }
$client = $null
$deadline = (Get-Date).AddSeconds(120)
while (-not $client) {
    try { $client = New-Object System.Net.Sockets.TcpClient("127.0.0.1", $Port) }
    catch { if ((Get-Date) -gt $deadline) { throw "no stub" }; Start-Sleep -Milliseconds 500 }
}
$st = $client.GetStream()
$st.ReadTimeout = 30000
function Send([string]$p) { $b = [Text.Encoding]::ASCII.GetBytes('$' + $p + '#' + (Csum $p)); $st.Write($b, 0, $b.Length) }
function Recv([bool]$started = $false) {
    $sb = New-Object Text.StringBuilder; $in = $started
    while ($true) {
        $c = $st.ReadByte(); if ($c -lt 0) { throw "closed" }
        $ch = [char]$c
        if (-not $in) { if ($ch -eq '$') { $in = $true }; continue }
        if ($ch -eq '#') { [void]$st.ReadByte(); [void]$st.ReadByte(); $ack = [byte][char]'+'; $st.WriteByte($ack); return $sb.ToString() }
        [void]$sb.Append($ch)
    }
}
function Ask([string]$p) { Send $p; Recv }
$lines = New-Object Collections.Generic.List[string]
$lines.Add("stop: " + (Ask "?"))
function Drain { Start-Sleep -Milliseconds 300; while ($st.DataAvailable) { $x = $st.ReadByte(); if ($x -eq 36) { $lines.Add("drained: " + (Recv $true)) } } }
function Chain([uint32]$sp) {
    $out = @()
    for ($i = 0; $i -lt 12 -and $sp -ne 0 -and $sp -ne 0xffffffff; $i++) {
        $m = Ask ("m{0:x},8" -f $sp); if ($m.Length -lt 16) { break }
        $out += $m.Substring(8, 8); $sp = [Convert]::ToUInt32($m.Substring(0, 8), 16)
    }
    $out -join " "
}
$hits = 0
if ($Watch -ne "") { $lines.Add("watch " + $Watch + ": " + (Ask ("Z2," + $Watch + ",4"))) }
Send "c"
$deadline = (Get-Date).AddSeconds($Timeout); $hit = $false
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 200
    if ($Watch -ne "" -and $st.DataAvailable) {
        if ($st.ReadByte() -ne 36) { continue }   # '+' acks of our packets
        $stop = Recv $true
        $lines.Add("raw stop: $stop"); Set-Content -Path $Out -Value $lines
        Drain
        $hits++
        $lines.Add(("hit {0}: {1} pc {2} lr {3} value {4} chain {5}" -f $hits, $stop, (Ask "p40"), (Ask "p43"), (Ask ("m" + $Watch + ",4")), (Chain ([Convert]::ToUInt32((Ask "p1"), 16)))))
        Set-Content -Path $Out -Value $lines
        if ($hits -ge $MaxHits) { break }
        Send "c"
        continue
    }
    try {
        $fs = [IO.File]::Open($Log, 'Open', 'Read', 'ReadWrite')
        $sr = New-Object IO.StreamReader($fs); $t = $sr.ReadToEnd(); $sr.Close()
        if ($t -match $Pattern) { $hit = $true; break }
    } catch {}
}
$lines.Add("hit: $hit")
$st.WriteByte(3)
$lines.Add("break: " + (Recv))
Start-Sleep -Milliseconds 800
while ($st.DataAvailable) { $extra = $st.ReadByte(); if ($extra -eq 36) { $lines.Add("drained: " + (Recv $true)) } }
foreach ($r in @(0..31 + 64, 65, 66, 67, 68)) { $lines.Add(("reg {0} {1}" -f $r, (Ask ("p{0:x}" -f $r)))) }
$sp = [Convert]::ToUInt32((Ask "p1"), 16)
for ($i = 0; $i -lt 24 -and $sp -ne 0 -and $sp -ne 0xffffffff; $i++) {
    $m = Ask ("m{0:x},8" -f $sp)
    $lines.Add(("frame {0:x8} {1}" -f $sp, $m))
    if ($m.Length -lt 16) { break }
    $sp = [Convert]::ToUInt32($m.Substring(0, 8), 16)
}
$lines.Add("pcwords " + (Ask ("m{0:x},40" -f ([Convert]::ToUInt32((Ask "p40"), 16) - 32))))
Set-Content -Path $Out -Value $lines
Send "k"
$client.Close()
