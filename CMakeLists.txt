cmake_minimum_required(VERSION 3.10)

# project name
project(CPPAlgorithms)

# c++ version
set(CMAKE_CXX_STANDARD 17)

# 0. Add executable for demo
add_executable(demo main.cpp)

# 1. Add executable for main2
add_executable(ParsingArithmeticExpressionsIntoAST ParsingArithmeticExpressionsIntoAST.cpp)