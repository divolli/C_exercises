cmake_minimum_required(VERSION 3.10)
project(MainCppProject C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

add_executable(main main.c)
