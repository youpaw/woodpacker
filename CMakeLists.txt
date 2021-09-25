cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_STANDARD 99)
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

project(woody_woodpacker)

set(LIBFT_DIR "libft")
include_directories(${LIBFT_DIR})
add_subdirectory(${LIBFT_DIR})

include_directories(src src/load src/crypt src/pack src/pack/elf64)
add_executable(${PROJECT_NAME} src/woodpacker.c
        src/load/load_exec.c
        src/load/new_exec_data.c
        src/load/del_exec_data.c
        src/load/validate_exec_elf64.c
        src/load/validate_exec.c
        src/crypt/crypt_exec.c
        src/pack/pack_exec.c
        src/pack/elf64/pack_elf64.c
        src/pack/get_map_data.c
        src/pack/elf64/init_tmpl_elf64.c
        src/pack/elf64/write_woody_elf64.c src/pack/elf64/adjust_tmpl_vars_elf64.c src/pack/elf64/get_symtab_shdr_elf64.c src/pack/elf64/get_strtab_ptr_elf64.c)


SET(WOODY_TMPL_NAME "woody_tmpl")
add_executable(${WOODY_TMPL_NAME} src/woody.c)
target_link_libraries(${WOODY_TMPL_NAME} PUBLIC "-static")

set(TEMPLATES_DIR "templates")
file(MAKE_DIRECTORY ${TEMPLATES_DIR})

SET(CREAT_WOODY_TMPL_TARGET "copy_woody_tmpl")
add_custom_target(${CREAT_WOODY_TMPL_TARGET} DEPENDS ${WOODY_TMPL_NAME})
add_custom_command(TARGET ${CREAT_WOODY_TMPL_TARGET}
        COMMAND mv
        ${CMAKE_BINARY_DIR}/${WOODY_TMPL_NAME}
        ${CMAKE_SOURCE_DIR}/${TEMPLATES_DIR}/woody_elf64)

target_link_libraries(${PROJECT_NAME} ft)
add_dependencies(${PROJECT_NAME} ${CREAT_WOODY_TMPL_TARGET})