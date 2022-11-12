cmake_minimum_required(VERSION 3.21)
project(ImageCartoonizer)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        main.cpp src/Image.hpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ./libs/include src)
