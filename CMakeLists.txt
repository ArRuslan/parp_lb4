cmake_minimum_required(VERSION 3.29)
project(parp_lb4_6)

set(CMAKE_CXX_STANDARD 20)

add_executable(parp_lb4_6 main.cpp)
target_link_libraries(parp_lb4_6 PRIVATE gomp)
target_compile_options(parp_lb4_6 PRIVATE "-fopenmp")