cl /DEBUG /Zi /EHsc /FeGuitarTunerWin.exe /DBUILD_DEBUG winmain.cpp detect_usb.cpp SDL2-2.0.14\lib\x86\SDL2.lib portaudio_x86.lib imgui\*.cpp imgui_sdl.cpp audio.cpp SDL2_image-2.0.5\lib\x86\SDL2_Image.lib /ISDL2-2.0.14\include /Iimgui /Iimgui_sdl /IPortAudio\include /ISDL2_image-2.0.5\include /link GuitarTunerWin.res user32.lib
copy SDL2-2.0.14\lib\x86\SDL2.dll .\SDL2.dll
copy SDL2_image-2.0.5\lib\x86\*.dll .