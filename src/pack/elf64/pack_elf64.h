//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H
#define WOODY_TMPL_ELF64 "templates/woody_elf64"
#define DATA_SEGMENT_OFFSET 0x0000000000003000
#define DATA_VIRTUAL_ADDRESS 0x0000000000403000

#include "pack_exec.h"
#include <unistd.h>
#include <elf.h>

typedef struct s_data_phdr{
	Elf64_Phdr *phdr;
	off_t off;
}				t_data_phdr;

int			pack_elf64(const t_map_data *exec);

t_data_phdr *new_data_phdr(const t_map_data *tmpl, const t_map_data *exec);
void 		del_data_phdr(t_data_phdr **data_header);

int			adjust_tmpl_elf64(t_map_data *tmpl, const t_map_data *exec);
int			write_woody_elf64(const t_map_data *tmpl, const t_map_data *exec);
#endif //PACK_ELF64_H
