cmake_minimum_required(VERSION 3.16.3)
project(Colomb)

add_library(${PROJECT_NAME} src/Colomb.cpp)
include_directories(${PROJECT_NAME} PRIVATE include)
include_directories(${PROJECT_NAME} PRIVATE ../BitStream/include)