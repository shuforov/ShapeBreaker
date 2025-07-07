# ShapeBreaker/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ShapeBreaker)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ShapeBreaker src/main.cpp)

target_link_libraries(ShapeBreaker sfml-graphics sfml-window sfml-system)
