cmake_minimum_required(VERSION 3.5)
project (svmsg)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("../include")
include_directories("../svsem")

link_directories("../lib")
link_directories("../svsem")

add_executable(svshm_xfr_writer svshm_xfr_writer.c)
add_executable(svshm_xfr_reader svshm_xfr_reader.c)

target_link_libraries(svshm_xfr_writer tlpi binary_sems)
target_link_libraries(svshm_xfr_reader tlpi binary_sems)
