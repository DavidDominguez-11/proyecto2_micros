cmake_minimum_required(VERSION 3.28)
project(Principal C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)
if(OpenMP_C_FOUND)
    target_link_libraries(Principal PUBLIC OpenMP::OpenMP_C)
endif()

add_executable(Principal main.c)
