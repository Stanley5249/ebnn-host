cmake_minimum_required(VERSION 3.10)
project(app VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(example_glfw_opengl3 examples/example_glfw_opengl3.cpp)

target_link_libraries(example_glfw_opengl3 PRIVATE imgui::imgui glfw OpenGL::GL)

install(TARGETS example_glfw_opengl3 DESTINATION bin)
