cmake_minimum_required(VERSION 3.16.3)
project(testBitStream)
add_subdirectory(BitStream)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE BitStream/include)
target_link_libraries(${PROJECT_NAME} BitStream)