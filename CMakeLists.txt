cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_STANDARD 99)

project(woody_woodpacker)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(include)
add_executable(${PROJECT_NAME} woodpacker.c)

target_link_libraries(${PROJECT_NAME} ft)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall -Wextra -Werror")