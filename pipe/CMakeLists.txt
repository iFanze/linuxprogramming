cmake_minimum_required(VERSION 3.5)
project (pipe)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("../include")

link_directories("../lib")

add_executable(fifo_seqnum_server fifo_seqnum_server.c)
add_executable(fifo_seqnum_client fifo_seqnum_client.c)

target_link_libraries(fifo_seqnum_server tlpi)
target_link_libraries(fifo_seqnum_client tlpi)
