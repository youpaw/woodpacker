//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H

#include "pack_exec.h"
#include <unistd.h>

/*
 * PRINT_WOODY_ELF64 size = 58
 */

#define PRINT_WOODY_ELF64	"\xE8\x0E\x00\x00\x00\x2E\x2E\x2E\x2E\x57\x4F\x4F" \
							"\x44\x59\x2E\x2E\x2E\x2E\x0A\xB8\x01\x00\x00\x00" \
							"\xBF\x01\x00\x00\x00\x5E\xBA\x0E\x00\x00\x00\x0F" \
							"\x05\x48\x31\xC0\x48\x31\xFF\x48\x31\xD2\x48\x31" \
							"\xF6\xEB\x00\x00\x00\x00\x00\x00\x00\x00"

#define PAYLOAD_ELF64 PRINT_WOODY_ELF64
#define PAYLOAD_SIZE_ELF64 58

#define JMP_ADDR_OFF_ELF64 50

t_data_wrap	*pack_elf64(const t_exec_map *exec);
t_data_wrap *assemble_woody_elf64(t_data_wrap *woody, t_cave_info *cave_info);

#endif //PACK_ELF64_H
