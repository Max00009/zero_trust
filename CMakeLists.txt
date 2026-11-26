# This is the root(apex) CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(zero_trust CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Defines Threads::Threads (needed on older glibc). Thread-using targets must link it.
find_package(Threads REQUIRED)

# Subdirectories to add
#add_subdirectory(modules/metadata_sweeper)
add_subdirectory(modules/traffic_shield)