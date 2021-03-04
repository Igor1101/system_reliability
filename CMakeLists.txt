cmake_minimum_required(VERSION 3.14)

project(system_reliability LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_FILES system_reliability_uk_UA.ts)

add_executable(system_reliability
  main.cpp
  graph.h
)
target_link_libraries(system_reliability )

