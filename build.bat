
cls

del VolumeApp.exe
del VolumeApp.obj
del OGL.res
del ".\\bin\\*.obj"


@REM cl.exe /c /EHsc /I ".\\dependencies\\glew\\include\\GL"  VolumeApp.cpp

@REM Compiles all the available cpp files in the subdirectories
for /r %%i in (*.cpp) do (
	echo "%%i"
    cl.exe /Fo".\\bin\\" /c /EHsc  /I ".\\dependencies\\glew\\include\\GL" "%%i"
)

echo ====================== COMPILATION DONE ======================

@REM rc.exe OGL.rc
rc.exe /Fo".\\bin\\OGL.res" OGL.rc

@REM link.exe VolumeApp.obj OGL.res user32.lib gdi32.lib /LIBPATH:".\\dependencies\\glew\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS


link.exe /out:VolumeApp.exe ./bin/*.obj ./bin/OGL.res user32.lib gdi32.lib /LIBPATH:".\\dependencies\\glew\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS

echo ====================== LINKING DONE ======================

VolumeApp.exe


 