cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)

# Project name and version
project(RayTracer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ensure the output directory exists
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Add the output directory to the include path
# target_include_directories(raytracer PRIVATE ${SHADER_OUTPUT_DIR})

# Add source files
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Check if source files were found, and if not, throw an error
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found. Check your src/ directory.")
endif()

# Automatically set build directories
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/shaders")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add shader directory as a macro
add_compile_definitions(SHADER_DIR="${SHADER_DIR}/")

# Generate shader header files for WASM
file(READ "${SHADER_DIR}/frag_tex.glsl" FRAG_TEX_SHADER)
file(READ "${SHADER_DIR}/frag_trace.glsl" FRAG_TRACE_SHADER)
file(READ "${SHADER_DIR}/vert_pass.glsl" VERT_PASS_SHADER)

configure_file(
    ${SHADER_DIR}/wasm_shaders.h.in
    ${SHADER_DIR}/wasm_shaders.h
    @ONLY
)

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please use a build directory like ${BUILD_DIR}.")
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Use pkg-config to find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

find_package(GLEW REQUIRED)

# Add include directory for header files
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/shaders)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link GLFW and OpenGL
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} OpenGL::GL GLEW::GLEW)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})
