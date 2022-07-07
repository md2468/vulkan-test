cmake_minimum_required(VERSION 3.10)
project(project)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_COMPILER g++)
set(Flags "-Wall" "-Wextra" "-Wpedantic" "-O3" "-std=c++20" "-fsanitize=address,undefined" "-g" "-rdynamic")
add_compile_options(${Flags})
add_link_options(${Flags})

file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "code/*.cpp")
add_executable(main ${Sources})

target_include_directories(main PUBLIC code)
target_link_libraries(main -ldl -lglfw -lvulkan -lshaderc_shared)

