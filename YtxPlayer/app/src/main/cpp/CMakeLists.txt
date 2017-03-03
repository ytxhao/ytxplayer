cmake_minimum_required(VERSION 3.4.1)

# now build app's shared lib
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

include_directories(${PROJECT_SOURCE_DIR}/include)



add_subdirectory(gl-engine)
add_subdirectory(ytxplayer)
add_subdirectory(ffplay-test)



