# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.


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

set(INC_DIR "${Project_SOURCE_DIR}/src/main/cpp/include")
set(LINK_DIR "${Project_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a")

include_directories(${INC_DIR})
#link_directories(${LINK_DIR})
#set(CMAKE_LIBRARY_PATH ${LINK_DIR})


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
                       ${avutil-lib}
                       ${swscale-lib}
                       ${swresample-lib}
                       ${avcodec-lib}
                       ${avformat-lib}
                       ${avfilter-lib}
                       )
