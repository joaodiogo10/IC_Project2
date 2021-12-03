cmake_minimum_required(VERSION 3.16.3)
project(PartA_1)

add_executable(testBitStream testBitStream.cpp classes/BitStream.cpp)
target_include_directories(testBitStream PRIVATE classes)
target_link_directories(testBitStream PRIVATE classes)

add_executable(testGolomb testGolomb.cpp classes/BitStream.cpp classes/Golomb.cpp)
target_include_directories(testGolomb PRIVATE classes)
target_link_directories(testGolomb PRIVATE classes)
