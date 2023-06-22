cmake_minimum_required(VERSION 3.21)
project(Lab_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_2 main.cpp lexical_analyzer.cpp lexical_analyzer.h syntactic_analyzer.cpp syntactic_analyzer.h data.h data.cpp generator.cpp generator.h)
