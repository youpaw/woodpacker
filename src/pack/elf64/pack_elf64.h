//
// Created by youpaw on 21/08/2021.
//

#ifndef PACK_ELF64_H
#define PACK_ELF64_H
#define WOODY_TMPL_ELF64 "templates/woody_elf64"
#define STRTAB_NAME ".strtab"

#include "pack_exec.h"
#include <unistd.h>
#include <elf.h>

int			pack_elf64(const t_map_data *exec);

int			init_tmpl_elf64(const t_map_data *tmpl, const t_map_data *exec,
							   t_inject_info *inject);

Elf64_Shdr	*get_symtab_shdr_elf64(Elf64_Ehdr *ehdr);
char		*get_strtab_ptr_elf64(Elf64_Ehdr *ehdr);
int			adjust_tmpl_vars_elf64(const t_map_data *tmpl,
									  const t_inject_info *inject);
int			write_woody_elf64(const t_map_data *tmpl, const t_map_data *exec,
								 t_inject_info *inject);
#endif //PACK_ELF64_H
