cmake_minimum_required(VERSION 3.16.3)
project(BitStream)

add_library(${PROJECT_NAME} src/BitStream.cpp)
include_directories(${PROJECT_NAME} PRIVATE include)

