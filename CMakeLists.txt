cmake_minimum_required(VERSION 3.16)
project(MediaParser)

set(CMAKE_CXX_STANDARD 11)

add_executable(MediaParser main.cpp AACParser.cpp AACParser.h H264Parser.cpp H264Parser.h)