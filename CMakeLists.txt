cmake_minimum_required(VERSION 3.23)
project(CppProj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(CppProj main.cpp)
add_subdirectory(ThreadPool)
target_link_libraries(CppProj PUBLIC tp)
