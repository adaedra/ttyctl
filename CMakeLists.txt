cmake_minimum_required(VERSION 3.0)
project(ttyctl)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(ttyctl ttyctl.c)
