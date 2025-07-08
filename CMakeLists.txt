# ShapeBreaker/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ShapeBreaker)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Set output directory to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Gather all cpp files
file(GLOB SRC_FILES src/*.cpp)

# Create executable
add_executable(ShapeBreaker ${SRC_FILES})

# Include directories
target_include_directories(ShapeBreaker PRIVATE include)

# Find and link SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(ShapeBreaker sfml-graphics sfml-window sfml-system sfml-audio)

# Add "run" target
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ShapeBreaker
    DEPENDS ShapeBreaker
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
