cmake_minimum_required(VERSION 3.28)
project(ebnn-host VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C++20 modules support
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Find required packages
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Create whiteboard library using C++20 modules (single-partition style)
add_library(ebnn_host STATIC)

# Add module partitions
target_sources(ebnn_host
    PUBLIC
        FILE_SET modules TYPE CXX_MODULES FILES
            src/ebnn_host/ebnn_host.cppm
            src/ebnn_host/canvas.cppm
            src/ebnn_host/inference.cppm
            src/ebnn_host/ui.cppm
)

# Create GLAD library as STATIC to avoid DLL dependency
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

target_link_libraries(ebnn_host
    PUBLIC
        imgui::imgui
        glad_gl_core_33
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
        ebnn_host
        glfw
)

# Install the executable files
install(TARGETS example_glfw_opengl3 example_whiteboard
    RUNTIME DESTINATION bin
)

# Install the shared library (DLL) that the executables need
install(TARGETS glad_gl_core_33
    RUNTIME DESTINATION bin
)
# Linting
install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    DESTINATION share/intellisense
)

install(DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ebnn_host.dir/
    DESTINATION share/ifc
    FILES_MATCHING PATTERN "*.ifc"
)

