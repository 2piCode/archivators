cmake_minimum_required(VERSION 3.5)

project(archivator VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HEADERS include/archivator.h
            include/haffman_archivator.h
)

set (SOURCES src/main.cpp)

add_executable(archivator ${SOURCES})

target_include_directories(archivator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
