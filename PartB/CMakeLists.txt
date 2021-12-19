cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
project( PartB )

add_executable(codecLossless codecLossless.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_include_directories(codecLossless PRIVATE ../PartA_1/classes)
target_link_directories(codecLossless PRIVATE ../PartA_1/classes)

add_executable(codecLossy codecLossy.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_include_directories(codecLossy PRIVATE ../PartA_1/classes)
target_link_directories(codecLossy PRIVATE ../PartA_1/classes)