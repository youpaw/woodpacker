cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_STANDARD 99)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

project(woody_woodpacker)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(include)
add_executable(${PROJECT_NAME} woodpacker.c)

target_link_libraries(${PROJECT_NAME} ft)