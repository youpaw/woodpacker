//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H
#define WOODY_TMPL_ELF64 "templates/woody_elf64"
#define DATA_PHDR_OFFSET 0
#define DATA_SEGMENT_OFFSET 0

#include "pack_share.h"
#include <unistd.h>

int	pack_elf64(const t_pack_data *data);
int	validate_exec_elf64(const void *exec_map, size_t size);
int insert_data_elf64(void *tmpl_map, const t_pack_data *data);
int write_woody_elf64(void *tmpl_map, off_t tmpl_size, const t_pack_data *data);
#endif //PACK_ELF64_H
