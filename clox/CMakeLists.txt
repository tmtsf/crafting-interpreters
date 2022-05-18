cmake_minimum_required(VERSION 3.12)
project(clox)

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
  set(CMAKE_CXX_FLAGS "/O2 /EHsc")
else()
  set(CMAKE_CXX_FLAGS "-O2")
endif()

include_directories(include)
add_subdirectory(src)
add_subdirectory(clox)
