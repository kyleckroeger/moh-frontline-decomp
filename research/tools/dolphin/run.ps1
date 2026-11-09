param($exe,$iso,$user,$secs,$out)
# Boot one disc image in Dolphin for $secs seconds, saving a window capture
# every 15 seconds as "$out-N.png". Called by research/tools/dolphin_boot.py.
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;
public class W{[DllImport("user32.dll")]public static extern bool PrintWindow(IntPtr h,IntPtr d,uint f);
[DllImport("user32.dll")]public static extern bool GetWindowRect(IntPtr h,out R r);
public struct R{public int L,T,Rt,B;}}
"@
$p=Start-Process -FilePath $exe -ArgumentList @('-b','-u',('"'+$user+'"'),'-e',('"'+$iso+'"')) -PassThru
$t0=Get-Date;$n=0
while(((Get-Date)-$t0).TotalSeconds -lt $secs){
 Start-Sleep -Seconds 15;$n++
 if($p.HasExited){"exited code $($p.ExitCode) at $(((Get-Date)-$t0).TotalSeconds)";break}
 $p.Refresh();$h=$p.MainWindowHandle
 if($h -ne 0){$r=New-Object W+R;[void][W]::GetWindowRect($h,[ref]$r);$w=$r.Rt-$r.L;$g=$r.B-$r.T
  if($w -gt 0){$b=New-Object Drawing.Bitmap $w,$g;$c=[Drawing.Graphics]::FromImage($b);$d=$c.GetHdc();[void][W]::PrintWindow($h,$d,2);$c.ReleaseHdc($d);$b.Save("$out-$n.png");$c.Dispose();$b.Dispose()}
  "t=$n title=[$($p.MainWindowTitle)] responding=$($p.Responding)"}
}
if(-not $p.HasExited){Stop-Process -Id $p.Id -Force;"stopped"}
