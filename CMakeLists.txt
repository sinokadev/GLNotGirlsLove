cmake_minimum_required(VERSION 3.10)

set(CMAKE_OPENGL_GL_PREFERENCE GLVND)
project(GLNotGirlsLove)

set(SOURCES 
    src/main.cpp 
    src/gl.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    OpenGL::GL 
    glfw
)