cmake_minimum_required(VERSION 3.10)
project(ebnn-host VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Create whiteboard library as STATIC
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(ebnn_whiteboard STATIC ${SOURCES})

# Create GLAD library as STATIC to avoid DLL dependency
glad_add_library(glad_gl_core_33 STATIC REPRODUCIBLE API gl:core=3.3)

target_include_directories(ebnn_whiteboard
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ebnn_whiteboard
    PUBLIC
        imgui::imgui
        glad_gl_core_33
        OpenGL::GL
)

# Create example executables
add_executable(example_glfw_opengl3 examples/example_glfw_opengl3.cpp)
target_link_libraries(example_glfw_opengl3
    PRIVATE
        imgui::imgui
        glfw
        OpenGL::GL
)

add_executable(example_whiteboard examples/example_whiteboard.cpp)
target_link_libraries(example_whiteboard 
    PRIVATE 
        ebnn_whiteboard
        glfw
)

# Install targets
install(TARGETS ebnn_whiteboard glad_gl_core_33 example_glfw_opengl3 example_whiteboard
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY include/ebnn_host DESTINATION include)
