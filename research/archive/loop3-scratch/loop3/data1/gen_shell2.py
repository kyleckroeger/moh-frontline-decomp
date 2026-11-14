src=open('gen_shell.py').read()
exec(src.split('for root in sys.argv')[0]+src.split("print(root,hex(s['address']),s['size'],out[:8])")[1])
W='init/'
b,t=libinfo('PauseLibraryInfo',None,None)
open(W+'ispauselibrary.cpp','w').write('''// ispauselibrary.cpp: the pause screen's image libraries (.data, .sdata), as the screen
// tool exported them: for each library its file names and a slot per file for the loaded
// object (zero until loaded), and PauseLibraryInfo, which lists the libraries.
// Checked by research/tools/check_data.py.

'''+LIB_VIEW+'\n'+'\n'.join(b)+'\n'+t)
b,t=libinfo('ShellLibraryInfo',None,None)
open(W+'isShellLibrary.cpp','w').write('''// isShellLibrary.cpp: the shell's image libraries (.data, .sdata), as the screen tool
// exported them: for each library its file names and a slot per file for the loaded
// object (zero until loaded), and ShellLibraryInfo, which lists the libraries. The file of
// ShellLibraryInfo itself is inferred (no file map range covers it; it follows the last
// library in .data). Checked by research/tools/check_data.py.

'''+LIB_VIEW+'\n'+'\n'.join(b)+'\n'+t)
open(W+'ispausegroup.cpp','w').write('''// ispausegroup.cpp: the pause screen group (.sdata): its screen files and PauseScreenInfo.
// The file of PauseScreenInfo is inferred (no file map range covers it; it follows
// szscreen_pauseexport in .sdata and points at it). Checked by research/tools/check_data.py.

'''+SCR_VIEW+'\n'+screen('PauseScreenInfo','szscreen_pauseexport'))
open(W+'isShellGroup.cpp','w').write('''// isShellGroup.cpp: the shell screen group (.data, .sdata): its screen files and
// ShellScreenInfo. The file of ShellScreenInfo is inferred (no file map range covers it;
// it points at szscreen_export). Checked by research/tools/check_data.py.

'''+SCR_VIEW+'\n'+screen('ShellScreenInfo','szscreen_export'))
