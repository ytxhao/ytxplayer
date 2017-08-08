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

set(LINK_DIR "${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}")


add_library( png-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       png-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libpng16.so )


add_library( harfbuzz-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       harfbuzz-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libharfbuzz.so )

add_library( freetype-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       freetype-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libfreetype.so )


add_library( iconv-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       iconv-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libiconv.so )


add_library( xml2-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       xml2-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libxml2.so )

add_library( fontconfig-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       fontconfig-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libfontconfig.so )


add_library( fribidi-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       fribidi-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libfribidi.so )

add_library( ass-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       ass-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libass.so )


add_library( avutil-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avutil-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libavutil-55.so )

add_library( swscale-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       swscale-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libswscale-4.so )

add_library( swresample-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       swresample-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libswresample-2.so )


add_library( avcodec-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avcodec-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libavcodec-57.so )

add_library( avformat-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avformat-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libavformat-57.so )

add_library( avfilter-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avfilter-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LINK_DIR}/libavfilter-6.so )
#-----------------------------------------------------------------
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       native-ytx-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       android
                       ${log-lib}
                       ${OpenSLES-lib}
                       native-sdl-lib
                       png-lib
                       harfbuzz-lib
                       freetype-lib
                       iconv-lib
                       xml2-lib
                       fontconfig-lib
                       fribidi-lib
                       ass-lib
                       avutil-lib
                       swscale-lib
                       swresample-lib
                       avcodec-lib
                       avformat-lib
                       avfilter-lib
                       native-gl-lib
                       native-sdl-lib
                       )