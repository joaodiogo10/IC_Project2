cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project( ex1 )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( ex1 ex1.cpp )
target_link_libraries( ex1 ${OpenCV_LIBS} )
