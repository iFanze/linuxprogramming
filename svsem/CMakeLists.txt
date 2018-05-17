cmake_minimum_required(VERSION 3.5)
project (svsem)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("../include")

link_directories("../lib")

add_library(binary_sems binary_sems.c)

target_link_libraries(binary_sems tlpi)
