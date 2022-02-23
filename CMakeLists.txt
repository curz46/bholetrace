cmake_minimum_required(VERSION 3.0)

project(bholetrace)

file(GLOB sources
    "src/*.cpp"
    "src/*.hpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(OpenGL_GL_PREFERENCE "LEGACY")

include_directories(include)
add_executable(bhole ${sources})

add_subdirectory(lib)
target_link_libraries(bhole stb_image glm)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(bhole ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(bhole ${OPENGL_LIBRARIES})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
target_link_libraries(bhole ${GLUT_LIBRARIES})

find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
target_link_libraries(bhole glfw)

add_custom_command(TARGET bhole PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink 
                   ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_command(TARGET bhole PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink 
                   ${CMAKE_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR}/textures)

#file(GLOB textures 
#    "textures/*.png")
#file(COPY ${textures} DESTINATION textures)
#
#file(GLOB shaders
#    "src/shaders/*")
#file(COPY ${shaders} DESTINATION shaders)
