cmake_minimum_required(VERSION 3.16)
project(PolynomialSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(polynomial_solver SHARED Code.cpp)
target_compile_definitions(polynomial_solver PRIVATE POLYNOMIAL_SOLVER_BUILD)
target_include_directories(polynomial_solver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(polynomial_solver_test main.cpp)
target_link_libraries(polynomial_solver_test PRIVATE polynomial_solver)
