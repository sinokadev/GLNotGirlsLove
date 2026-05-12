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

file(GLOB COMMON_SOURCES "src/common/*.cpp" "library/*.c")

add_library(KetchupEngine STATIC ${COMMON_SOURCES})
target_include_directories(KetchupEngine PRIVATE include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(KetchupEngine
    PUBLIC 
    OpenGL::GL 
    glfw
    glm::glm
)

macro(add_demo demo_name main_source)
    add_executable(${demo_name} ${main_source})
    target_link_libraries(${demo_name} PRIVATE KetchupEngine)
    target_include_directories(${demo_name} PRIVATE include)
endmacro()

add_demo(cheese_block src/cheese_block.cpp)
add_demo(phong_asphalt src/phong_asphalt.cpp)