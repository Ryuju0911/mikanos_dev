cmake_minimum_required(VERSION 3.13)
project(unittest C CXX)
find_package(GTest)
add_subdirectory(kernel)
add_subdirectory(test)