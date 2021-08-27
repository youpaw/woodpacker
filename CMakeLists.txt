cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_STANDARD 99)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

project(woody_woodpacker)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(src src/pack src/pack/pack_elf64)
add_executable(${PROJECT_NAME} src/woodpacker.c src/validate_exec.c src/load_exec.c src/new_exec_data.c src/del_exec_data.c src/pack/pack_elf64/validate_exec_elf64.c src/pack/pack_elf64/pack_elf64.c src/pack/pack_share.h src/get_pack_data.c src/pack_exec.c)

target_link_libraries(${PROJECT_NAME} ft)