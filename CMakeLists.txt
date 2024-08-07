cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_STANDARD 99)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

project(woody_woodpacker C ASM_NASM)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(src src/load src/crypt src/pack src/pack/elf64 src/pack/elf32 src/hash src/compress)
add_executable(${PROJECT_NAME}
        src/woodpacker.c
        src/new_data_wrap.c
        src/del_data_wrap.c
        src/crypt/encrypt.asm
        src/crypt/generate_key.c
        src/compress/compress.c
        src/load/load_exec.c
        src/load/mmap_exec.c
        src/load/munmap_exec.c
        src/load/validate_exec.c
        src/load/validate_exec_elf32.c
        src/load/validate_exec_elf64.c
        src/pack/pack_exec.c
        src/pack/write_woody.c
        src/pack/allocate_woody.c
        src/pack/elf32/pack_elf32.c
        src/pack/elf32/find_cave_elf32.c
        src/pack/elf32/assemble_woody_elf32.c
        src/pack/elf64/pack_elf64.c
        src/pack/elf64/find_cave_elf64.c
        src/pack/elf64/assemble_woody_elf64.c
        src/pack/del_cave_info.c)

target_link_libraries(${PROJECT_NAME} ft)
