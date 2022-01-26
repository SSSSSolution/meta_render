CMAKE_MINIMUM_REQUIRED(VERSION 3.4.0)

project(meta_render)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Vulkan REQUIRED FATAL_ERROR)

add_library(meta_render
    src/render_task.cpp
    src/renderer.cpp
    src/render_device.cpp
    src/vulkan_utils.cpp
)
target_include_directories(meta_render PRIVATE include)
target_include_directories(meta_render PRIVATE ${Vulkan_INCLUDES_DIRS})

target_link_libraries(meta_render Vulkan::Vulkan)

add_executable(meta_render_test
    src/test/main.cpp
    src/renderer.cpp
    src/vulkan_utils.cpp
    src/test/windows_window.cpp
)
target_include_directories(meta_render_test PRIVATE include)
target_include_directories(meta_render_test PRIVATE ${Vulkan_INCLUDES_DIRS})

target_link_libraries(meta_render_test Vulkan::Vulkan)


