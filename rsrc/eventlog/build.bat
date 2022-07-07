@REM Search Message Compiler from Program Files
for /f "delims=" %%i in ('dir /s /b ^
                            "C:\Program Files (x86)\Windows Kits" ^
                            "C:\Program Files\Windows Kits" ^
                            2^>nul ^|^
                        findstr /r /c:"\\mc.exe$" ^|^
                        findstr x64') do set "mc_exe=%%i"
@REM Compiler *.man file
"%mc_exe%" -um Yama.Events.man

@REM Search Resource Compiler from Program Files
for /f "delims=" %%i in ('dir /s /b ^
                            "C:\Program Files (x86)\Windows Kits" ^
                            "C:\Program Files\Windows Kits" ^
                            2^>nul ^|^
                        findstr /r /c:"\\rc.exe$" ^|^
                        findstr x64') do set "rc_exe=%%i"

@REM Compiler *.rc file
"%rc_exe%" Yama.Events.rc

@REM Search MSVC linker from Program Files
for /f "delims=" %%i in ('dir /s /b ^
                            "C:\Program Files (x86)\Microsoft Visual Studio" ^
                            "C:\Program Files\Microsoft Visual Studio" ^
                            2^>nul ^|^
                        findstr /r /c:"\\link.exe$" ^|^
                        findstr x64\\x64') do set "link_exe=%%i"


@REM Link resource files into empty DLL file.
"%link_exe%" /NOENTRY /DLL /MACHINE:X86 /out:Yama.Events.dll Yama.Events.res

@REM clean up intermidiate objects
echo y | del *.bin *.BIN *.rc *.res
