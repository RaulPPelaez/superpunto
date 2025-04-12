cmake_minimum_required(VERSION 3.25)
project(spunto LANGUAGES CXX C)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Set C++ standard and basic compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define application-specific compile definitions
set(MAXLOGLEVEL 5)
add_compile_definitions(MAXLOGLEVEL=${MAXLOGLEVEL})

add_compile_definitions(USEFONT=UbuntuMono-Regular.ttf)
include(CMakeRC)
file(GLOB_RECURSE RESOURCES RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/resources/UbuntuMono-Regular.ttf")
foreach(RESOURCE ${RESOURCES})
  message(STATUS "Found resource: ${RESOURCE}")
endforeach()
cmrc_add_resource_library(resources ALIAS resources::rc NAMESPACE superpunto_resources ${RESOURCES})


# Ensure both source and generated files are found by the compiler
include_directories(${CMAKE_SOURCE_DIR}/src)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(PNG REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
find_package(OpenGL REQUIRED OpenGL GLX)
message(STATUS "OpenGL include dirs: ${OPENGL_INCLUDE_DIRS}")
include_directories(${OPENGL_INCLUDE_DIRS})
find_package(Glad REQUIRED)
glad_add_library(glad_gl_core_45 SHARED API gl:core=4.5)

include_directories(${CMAKE_SOURCE_DIR}/src/third_party)

set(SOURCES
  src/App.cpp
  src/RParticleRenderer.cpp
  src/RWindow.cpp
  src/Camera.cpp
  src/RBox.cpp
  src/RPNG.cpp
  src/RFile.cpp
  src/RRenderer.cpp
  src/math_helper.cpp
  src/RGL.cpp
  src/RTextRenderer.cpp
  src/System.cpp)

add_executable(tohex tools/tohex.cpp)

file(GLOB SHADER_FILES "${PROJECT_SOURCE_DIR}/src/shaders/*")
set(GENERATED_FILES ${CMAKE_BINARY_DIR}/shaders.h)

add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND ${CMAKE_COMMAND} -E echo "Running tohex..."
    COMMAND $<TARGET_FILE:tohex> ${SHADER_FILES} -o ${GENERATED_FILES}
    DEPENDS ${SHADER_FILES} tohex
    COMMENT "Generating shader files"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
)

add_custom_target(generate_shaders ALL DEPENDS ${GENERATED_FILES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(LIB_LIST "")
foreach(SOURCE ${SOURCES})
  get_filename_component(LIB_NAME ${SOURCE} NAME_WE)
  add_library(${LIB_NAME} OBJECT ${SOURCE})
  target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  add_dependencies(${LIB_NAME} generate_shaders)
  list(APPEND LIB_LIST ${LIB_NAME})
  target_link_libraries(${LIB_NAME} PRIVATE SDL2::SDL2 SDL2_ttf OpenGL::OpenGL OpenGL::GLU OpenGL::GLX pthread png resources::rc glad_gl_core_45 glad)
endforeach()

# Create the spunto executable using main.cpp
add_executable(spunto src/main.cpp)
target_include_directories(spunto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spunto PRIVATE ${LIB_LIST} SDL2::SDL2 SDL2_ttf OpenGL::OpenGL OpenGL::GLU OpenGL::GLX pthread png resources::rc glad_gl_core_45 glad)
