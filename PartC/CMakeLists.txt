cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
project( ex1 )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( encoder encoder.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( encoder ${OpenCV_LIBS} )

add_executable( decoder decoder.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( decoder ${OpenCV_LIBS} )
