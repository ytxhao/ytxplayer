# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

#include_directories(src/main/cpp/include)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/native-lib.cpp )

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
#----------------------------------------------------------------
#set(JIN_DEFINITIONS src/main/jniLibs/armeabi-v7a/)
#LINK_DIRECTORIES(src/main/jniLibs/armeabi-v7a/)
#find_library(JIN_DEPEND
#              NAMES "avutil" "avformat" "swscale"
#              PATHS "${Project_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a"
#              NO_CMAKE_FIND_ROOT_PATH
#              )

set(INC_DIR src/main/cpp/include)
set(LINK_DIR src/main/jniLibs/armeabi-v7a)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

find_library(avutil
              NAMES "avutil"
              PATHS ${LINK_DIR}
              CMAKE_FIND_ROOT_PATH_BOTH
              )

find_library(swscale
            NAMES "swscale"
            PATHS ${LINK_DIR}
            CMAKE_FIND_ROOT_PATH_BOTH
            )

find_library(swresample
              NAMES "swresample"
              PATHS ${LINK_DIR}
              CMAKE_FIND_ROOT_PATH_BOTH
              )
find_library(avcodec
            NAMES "avcodec"
            PATHS ${LINK_DIR}
            CMAKE_FIND_ROOT_PATH_BOTH
            )
find_library(avformat
              NAMES "avformat"
              PATHS ${LINK_DIR}
              CMAKE_FIND_ROOT_PATH_BOTH
              )

find_library(avfilter
              NAMES "avfilter"
              PATHS ${LINK_DIR}
              CMAKE_FIND_ROOT_PATH_BOTH
              )

#            find_library(JIN_DEPEND
#                        NAMES avutil swscale swresample
#                        PATHS src/main/jniLibs/armeabi-v7a/ )

#SET(CMAKE_MODULE_PATH src/main/cpp/)
#find_package(FF REQUIRED)


#-----------------------------------------------------------------
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${avutil}
                       ${swscale}
                       ${swresample}
                       ${avcodec}
                       ${avformat}
                       ${avfilter}
                       )
