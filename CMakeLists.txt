cmake_minimum_required(VERSION 3.29)
project(parp_lb4)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(parp_lb4 main.cpp)
if (OPENMP_FOUND)
    target_compile_options(parp_lb4 PRIVATE "${OpenMP_CXX_FLAGS}")
    target_link_options(parp_lb4 PRIVATE "${OpenMP_EXE_LINKER_FLAGS}")
    target_link_libraries(parp_lb4 PRIVATE "${OpenMP_CXX_LIBRARIES}")
endif()
