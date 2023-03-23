cmake_minimum_required(VERSION 3.13)
project(kernel)
add_library(kernel STATIC
  graphics.cpp
  font.cpp
)