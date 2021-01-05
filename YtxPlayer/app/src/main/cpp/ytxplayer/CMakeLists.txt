# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

add_library( # Sets the name of the library.
             native-ytx-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/android_media_YtxMediaPlayer.cpp
             src/YtxMediaPlayer.cpp
             src/Thread.cpp
             src/PacketQueue.cpp
             src/Decoder.cpp
             src/decoder_video.cpp
             src/decoder_audio.cpp
             src/decoder_subtitle.cpp
             src/frame_queue.cpp
             src/frame_queue_audio.cpp
             src/frame_queue_video.cpp
             src/frame_queue_subtitle.cpp
             src/audio_engine.cpp
             src/lock.cpp
             src/VideoRefreshController.cpp
             src/VideoStateInfo.cpp
             src/playerPrepareAsync.cpp
             src/MessageQueue.cpp
             src/MessageLoop.cpp
             src/AudioRefreshController.cpp
             src/utils.cpp
             src/GLThread.cpp
             src/GlslFilter.cpp
             src/test.c
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

find_library( # Sets the name of the path variable.
            OpenSLES-lib

            # Specifies the name of the NDK library that
            # you want CMake to locate.
            OpenSLES )

#----------------------------------------------------------------

set(LINK_DIR "${PROJECT_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI}")

#-----------------------------------------------------------------
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

#libass.a             libfaad_drm.a        libmp3lame.a
#libavcodec.a         libfdk-aac.a         libpng16.a
#libavfilter.a        libfontconfig.a      librtmp.a
#libavformat.a        libfreetype.a        libssl.a
#libavutil.a          libfribidi.a         libswresample.a
#libcharset.a         libharfbuzz-subset.a libswscale.a
#libcrypto.a          libharfbuzz.a        libx264.a
#libfaad.a            libiconv.a           libxml2.a

target_link_libraries( # Specifies the target library.
                        native-ytx-lib

                        # Links the target library to the log library
                        # included in the NDK.
                        android
                        ${log-lib}
                        ${OpenSLES-lib}
                        native-gl-lib
                        native-sdl-lib
                        ${LINK_DIR}/libavfilter.a
                        ${LINK_DIR}/libavformat.a
                        ${LINK_DIR}/libavcodec.a
                        ${LINK_DIR}/libswresample.a
                        ${LINK_DIR}/libswscale.a
                        ${LINK_DIR}/libavutil.a
                        ${LINK_DIR}/libass.a
                        ${LINK_DIR}/libfontconfig.a
                        ${LINK_DIR}/libfreetype.a
                        ${LINK_DIR}/libharfbuzz.a
                        ${LINK_DIR}/libharfbuzz-subset.a
                        ${LINK_DIR}/libxml2.a
                        ${LINK_DIR}/libcharset.a
                        ${LINK_DIR}/libiconv.a
                        ${LINK_DIR}/libfaad.a
                        ${LINK_DIR}/libfaad_drm.a
                        ${LINK_DIR}/libfdk-aac.a
                        ${LINK_DIR}/libx264.a
                        ${LINK_DIR}/librtmp.a
                        ${LINK_DIR}/libpng16.a
                        ${LINK_DIR}/libfribidi.a
                        ${LINK_DIR}/libmp3lame.a
                        ${LINK_DIR}/libssl.a
                        ${LINK_DIR}/libcrypto.a
                        -lm
                        -lz
                       )