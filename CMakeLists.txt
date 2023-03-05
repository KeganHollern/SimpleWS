cmake_minimum_required(VERSION 3.23)
project(SimpleWS)

set(CMAKE_CXX_STANDARD 17)

add_library(SimpleWS SHARED simplews.cpp)

include_directories(include)