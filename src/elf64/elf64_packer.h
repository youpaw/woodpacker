//
// Created by youpaw on 21/08/2021.
//

#ifndef ELF64_PACKER_H
#define ELF64_PACKER_H

#include <unistd.h>

int validate_exec_elf64(const void *exec_map, size_t size);

#endif //ELF64_PACKER_H
