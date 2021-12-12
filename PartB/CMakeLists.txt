cmake_minimum_required(VERSION 3.16.3)
project(PartB)

add_executable(ex1 ex1.cpp classes/BitStream.cpp classes/Golomb.cpp)
target_include_directories(ex1 PRIVATE classes)
target_link_directories(ex1 PRIVATE classes)
