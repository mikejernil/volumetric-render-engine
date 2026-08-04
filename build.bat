
del VolumeApp.exe
del VolumeApp.obj
del OGL.res


cl.exe /c /EHsc /I "C:\\glew\\include" VolumeApp.cpp

rc.exe OGL.rc

link.exe VolumeApp.obj OGL.res user32.lib gdi32.lib /LIBPATH:"C:\\glew\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS

VolumeApp.exe


 