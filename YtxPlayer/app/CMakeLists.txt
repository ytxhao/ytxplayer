# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.



add_library(gl2jni-lib SHARED
            src/main/cpp/src/gl_code.cpp)

# add lib dependencies
target_link_libraries(gl2jni-lib
                      android
                      log
                      EGL
                      GLESv2)



add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/src/android_media_YtxMediaPlayer.cpp
             src/main/cpp/src/YtxMediaPlayer.cpp
             src/main/cpp/src/Thread.cpp
             src/main/cpp/src/PacketQueue.cpp
             src/main/cpp/src/Decoder.cpp
             src/main/cpp/src/decoder_video.cpp
             src/main/cpp/src/decoder_audio.cpp
             src/main/cpp/src/frame_queue.cpp
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
            EGL-lib

            # Specifies the name of the NDK library that
            # you want CMake to locate.
            EGL )

find_library( # Sets the name of the path variable.
            GLESv2-lib

            # Specifies the name of the NDK library that
            # you want CMake to locate.
            GLESv2 )
################################################################################


#set(LINK_DIR "${Project_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a")
#message(FATAL_ERROR "Invalid Android INC_DIR result: "${INC_DIR}")

include_directories(${Project_SOURCE_DIR}/src/main/cpp/include  ${Project_SOURCE_DIR}/src/main/cpp/include/ffmpeg ${Project_SOURCE_DIR}/src/main/cpp/include/ytxplayer)

################################################################################
add_library( avutil-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avutil-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libavutil.so )

################################################################################

add_library( swscale-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       swscale-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libswscale.so )

################################################################################

add_library( swresample-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       swresample-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libswresample.so )

################################################################################

add_library( avcodec-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avcodec-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libavcodec.so )

################################################################################

add_library( avformat-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avformat-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libavformat.so )

################################################################################

add_library( avfilter-lib
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       avfilter-lib

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       src/main/jniLibs/armeabi-v7a/libavfilter.so )

################################################################################

#-----------------------------------------------------------------
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${EGL-lib}
                       ${GLESv2-lib}
                       ${avutil-lib}
                       ${swscale-lib}
                       ${swresample-lib}
                       ${avcodec-lib}
                       ${avformat-lib}
                       ${avfilter-lib}
                       ${gl2jni-lib}
                       )


