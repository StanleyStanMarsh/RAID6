cmake_minimum_required(VERSION 3.24)
project(raid6_implementation)

set(CMAKE_CXX_STANDARD 20)

add_executable(raid6_implementation main.cpp Disk/Disk.cpp Disk/Disk.h RAID6/RAID6.cpp RAID6/RAID6.h)
