cmake_minimum_required(VERSION 3.0)

project(BHOLETRACE)

file(GLOB sources
    "src/*.cpp"
    "src/*.hpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(OpenGL_GL_PREFERENCE "LEGACY")

include_directories(include)
add_executable(bhole ${sources})

add_subdirectory(lib)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(bhole ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(bhole ${OPENGL_LIBRARIES})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
target_link_libraries(bhole ${GLUT_LIBRARIES})

file(GLOB resources
    "res/*.png")
file(COPY ${resources} DESTINATION res)

file(GLOB shaders
    "src/shaders/*")
file(COPY ${shaders} DESTINATION shaders)
