cmake_minimum_required(VERSION 3.5)
project (svmsg)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("../include")

link_directories("../lib")

add_executable(svmsg_file_server svmsg_file_server.c)
add_executable(svmsg_file_client svmsg_file_client.c)

target_link_libraries(svmsg_file_server tlpi)
target_link_libraries(svmsg_file_client tlpi)
