cmake_minimum_required(VERSION 3.10)

set(CMAKE_OPENGL_GL_PREFERENCE GLVND)
project(GLNotGirlsLove)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL glfw)