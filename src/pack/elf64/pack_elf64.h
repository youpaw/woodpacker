//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H

#include "pack_exec.h"
#include <unistd.h>

typedef struct	s_payload_elf64
{
	const unsigned char print[16];
	unsigned char jmp[5];
}				t_payload_elf64;

t_data_wrap	*pack_elf64(const t_exec_map *exec);
t_data_wrap *assemble_woody_elf64(t_exec_map *exec);

#endif //PACK_ELF64_H
