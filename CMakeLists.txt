cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(gamma)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WARNING_FLAGS -Wall -Wextra)
set(COVERAGE_FLAGS -fprofile-arcs -ftest-coverage)
set(OPTIMIZATION_FLAGS -O3 -mavx2 -fPIC)

enable_testing()

add_compile_options(${WARNING_FLAGS} ${COVERAGE_FLAGS} ${OPTIMIZATION_FLAGS})
add_link_options(${WARNING_FLAGS} ${COVERAGE_FLAGS} ${OPTIMIZATION_FLAGS})

add_subdirectory(src)
