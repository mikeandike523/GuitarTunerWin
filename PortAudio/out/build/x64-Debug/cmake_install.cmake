# Install script for directory: D:/Projects/GuitarTunerWin/PortAudio
# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
set(CMAKE_INSTALL_PREFIX "D:/Projects/GuitarTunerWin/PortAudio/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
if(BUILD_TYPE)
string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
else()
set(CMAKE_INSTALL_CONFIG_NAME "Debug")
endif()
message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()
# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
if(COMPONENT)
message(STATUS "Install component: \"${COMPONENT}\"")
set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
else()
set(CMAKE_INSTALL_COMPONENT)
endif()
endif()
# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
set(CMAKE_CROSSCOMPILING "FALSE")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/portaudio" TYPE FILE FILES "D:/Projects/GuitarTunerWin/PortAudio/README.md")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/portaudio" TYPE FILE FILES "D:/Projects/GuitarTunerWin/PortAudio/LICENSE.txt")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/portaudio-2.0.pc")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
"D:/Projects/GuitarTunerWin/PortAudio/include/portaudio.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_ds.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_waveformat.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_wmme.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_waveformat.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_wasapi.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_waveformat.h"
"D:/Projects/GuitarTunerWin/PortAudio/include/pa_win_wdmks.h"
)
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/portaudio_x64.lib")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/portaudio_x64.dll")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/portaudio_static_x64.lib")
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio/portaudioTargets.cmake")
file(DIFFERENT EXPORT_FILE_CHANGED FILES
"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio/portaudioTargets.cmake"
"D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/CMakeFiles/Export/lib/cmake/portaudio/portaudioTargets.cmake")
if(EXPORT_FILE_CHANGED)
file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio/portaudioTargets-*.cmake")
if(OLD_CONFIG_FILES)
message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio/portaudioTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
file(REMOVE ${OLD_CONFIG_FILES})
endif()
endif()
endif()
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio" TYPE FILE FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/CMakeFiles/Export/lib/cmake/portaudio/portaudioTargets.cmake")
if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio" TYPE FILE FILES "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/CMakeFiles/Export/lib/cmake/portaudio/portaudioTargets-debug.cmake")
endif()
endif()
if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/portaudio" TYPE FILE FILES
"D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/cmake/portaudio/portaudioConfig.cmake"
"D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/cmake/portaudio/portaudioConfigVersion.cmake"
)
endif()
if(CMAKE_INSTALL_COMPONENT)
set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()
string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
"${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/Projects/GuitarTunerWin/PortAudio/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
"${CMAKE_INSTALL_MANIFEST_CONTENT}")
