cmake_minimum_required(VERSION 3.28)
project(ebnn-host VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Dependenciespixi
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Main library: ebnn_host.lib (module partitions → .pcm files)
add_library(ebnn_host STATIC)
target_sources(ebnn_host
    PUBLIC
        FILE_SET modules TYPE CXX_MODULES FILES
            src/ebnn_host/ebnn_host.cppm
            src/ebnn_host/canvas.cppm
            src/ebnn_host/inference.cppm
            src/ebnn_host/ui.cppm
)

# GLAD OpenGL loader: creates glad_gl_core_33.lib + .dll
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

# ebnn_host links ImGui + GLAD (PUBLIC = transitive to dependents)
target_link_libraries(ebnn_host
    PUBLIC
        imgui::imgui
        glad_gl_core_33
)

# Example: standalone ImGui/GLFW demo → example_glfw_opengl3.exe
add_executable(example_glfw_opengl3 examples/example_glfw_opengl3.cpp)
target_link_libraries(example_glfw_opengl3
    PRIVATE
        imgui::imgui
        glfw
        OpenGL::GL
)

# Example: whiteboard app using ebnn_host → example_whiteboard.exe
add_executable(example_whiteboard examples/example_whiteboard.cpp)
target_link_libraries(example_whiteboard 
    PRIVATE 
        ebnn_host
        glfw
)

# Install: copy .exe files to bin/
install(TARGETS example_glfw_opengl3 example_whiteboard
    RUNTIME DESTINATION bin
)

# Install: copy GLAD .dll to bin/ (runtime dependency)
install(TARGETS glad_gl_core_33
    RUNTIME DESTINATION bin
)

# Dev tool: symlink compile_commands.json to project root
add_custom_command(
    TARGET ebnn_host POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
)