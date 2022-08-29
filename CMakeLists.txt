cmake_minimum_required(VERSION 3.17)
set(CMAKE_C_STANDARD 99)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

project(woody_woodpacker C)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(src src/load src/crypt src/pack src/pack/elf64)
add_executable(${PROJECT_NAME}
        src/woodpacker.c
        src/load/load_exec.c
        src/load/validate_exec.c
        src/load/validate_exec_elf64.c
        src/crypt/encrypt_aes.c
        src/pack/pack_exec.c
        src/pack/write_woody.c
        src/pack/elf64/pack_elf64.c
        src/pack/elf64/assemble_woody_elf64.c
        src/new_data_wrap.c src/del_data_wrap.c src/load/mmap_exec.c src/load/munmap_exec.c src/pack/elf64/find_cave_elf64.c)

target_link_libraries(${PROJECT_NAME} ft)
