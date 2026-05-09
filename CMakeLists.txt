cmake_minimum_required(VERSION 3.10)

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

set(CMAKE_OPENGL_GL_PREFERENCE GLVND)
project(GLNotGirlsLove)

file(GLOB SOURCES "src/*.c" "src/*.cpp")

add_executable(GLNotGirlsLove ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    OpenGL::GL 
    glfw
    glm::glm
)