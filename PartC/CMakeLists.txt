cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
project( ex1 )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( encoderLossless encoderLossless.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( encoderLossless ${OpenCV_LIBS} )

add_executable( decoderLossless decoderLossless.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( decoderLossless ${OpenCV_LIBS} )

add_executable( encoderLossy encoderLossy.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( encoderLossy ${OpenCV_LIBS} )

add_executable( decoderLossy decoderLossy.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_link_libraries( decoderLossy ${OpenCV_LIBS} )
