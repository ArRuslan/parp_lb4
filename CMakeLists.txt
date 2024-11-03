cmake_minimum_required(VERSION 3.29)
project(parp_lb4_7)

set(CMAKE_CXX_STANDARD 20)
find_package(PkgConfig REQUIRED)
pkg_check_modules(VIPS REQUIRED vips)

include_directories(${VIPS_INCLUDE_DIRS})

add_executable(parp_lb4_7 main.cpp)
target_link_libraries(parp_lb4_7 PRIVATE ${VIPS_LIBRARIES})
target_link_libraries(parp_lb4_7 PRIVATE gomp)
target_compile_options(parp_lb4_7 PRIVATE "-fopenmp")
target_compile_options(parp_lb4_7 PRIVATE "${VIPS_CFLAGS_OTHER}")
