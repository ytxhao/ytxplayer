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
             src/main/cpp/src/audio_engine.cpp
             src/main/cpp/src/gl_engine.cpp
             src/main/cpp/src/lock.cpp
             src/main/cpp/src/VideoRefreshController.cpp
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

#----------------------------------------------------------------

set(LINK_DIR "${Project_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a")


#message(FATAL_ERROR "Invalid Android INC_DIR result: "${INC_DIR}")
include_directories(${Project_SOURCE_DIR}/src/main/cpp/include  ${Project_SOURCE_DIR}/src/main/cpp/include/ffmpeg ${Project_SOURCE_DIR}/src/main/cpp/include/ytxplayer)




find_library(avutil-lib
                NAMES "avutil"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
              )

find_library(swscale-lib
                NAMES "swscale"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
            )

find_library(swresample-lib
                NAMES "swresample"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
              )
find_library(avcodec-lib
                NAMES "avcodec"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
            )
find_library(avformat-lib
                NAMES "avformat"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
              )

find_library(avfilter-lib
                NAMES "avfilter"
                PATHS ${LINK_DIR}
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_FIND_ROOT_PATH
              )


#-----------------------------------------------------------------
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

#MESSAGE(STATUS "lsx result: " "${LINK_DIR}/libavformat.so")
#message(FATAL_ERROR "Invalid Android lsx2 result: " "${avformat-lib}   ${LINK_DIR}")

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
                       OpenSLES )


