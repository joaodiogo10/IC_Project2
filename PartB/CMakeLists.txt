cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
project( PartB )

add_executable(ex1 ex1.cpp ../PartA_1/classes/BitStream.cpp ../PartA_1/classes/Golomb.cpp)
target_include_directories(ex1 PRIVATE ../PartA_1/classes)
target_link_directories(ex1 PRIVATE ../PartA_1/classes)
