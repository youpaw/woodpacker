//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H
#define WOODY_TMPL_ELF64 "templates/woody_elf64"

#include "pack_exec.h"
#include <unistd.h>
#include <elf.h>

int			pack_elf64(const t_map_data *exec);

int			adjust_tmpl_elf64(const t_map_data *tmpl, const t_map_data *exec,
								 t_inject_info *inject);
int			write_woody_elf64(const t_map_data *tmpl, const t_map_data *exec,
								 t_inject_info *inject);
#endif //PACK_ELF64_H
