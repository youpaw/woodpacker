//
// Created by youpaw on 11/09/2021.
//

#include "pack_elf64.h"
#include "string/ft_str.h"
#include "memory/ft_mem.h"
#include <stdio.h>
#include <errno.h>

static void	adjust_sym_val(Elf64_Ehdr *ehdr, Elf64_Sym *sym, size_t val)
{
	Elf64_Shdr *sym_shdr;
	size_t		val_off;
	size_t		sym_shdroff;

	sym_shdroff = ehdr->e_shoff + sym->st_shndx * ehdr->e_shentsize;
	sym_shdr = (Elf64_Shdr *)((char *)ehdr + sym_shdroff);
	val_off = sym_shdr->sh_offset + (sym->st_value - sym_shdr->sh_addr);
	ft_memcpy((char *)ehdr + val_off, &val, sizeof(size_t));
}

static Elf64_Sym	*get_sym_by_name(Elf64_Ehdr *ehdr, Elf64_Shdr *symtab_shdr,
							   const char *strtab, const char *symname)
{
	Elf64_Sym	*sym;
	size_t		symoff;

	symoff = symtab_shdr->sh_offset;
	while (symoff < symtab_shdr->sh_offset + symtab_shdr->sh_size)
	{
		sym = (Elf64_Sym *)((char *)ehdr + symoff);
		if (!ft_strcmp(symname, strtab + sym->st_name))
			return (sym);
		symoff += symtab_shdr->sh_entsize;
	}
	return (NULL);
}

static int	adjust_syms(Elf64_Ehdr *ehdr, Elf64_Shdr *symtab_shdr,
						   char *strtab, const size_t *inject_vals)
{
	static char	*sym_names[N_INJECT_SYMS] = {PAYLOAD_ADDR_SYM,
												PAYLOAD_SIZE_SYM};
	t_inject_sym	n_sym;
	Elf64_Sym		*sym;

	n_sym = 0;
	while (n_sym < N_INJECT_SYMS)
	{
		sym = get_sym_by_name(ehdr, symtab_shdr, strtab, sym_names[n_sym]);
		if (!sym)
		{
			errno = EINVAL;
			perror("Cannot find all inject symbols in woody elf64 template");
			return (-1);
		}
		adjust_sym_val(ehdr, sym, inject_vals[n_sym]);
		n_sym++;
	}
	return (0);
}

int	adjust_tmpl_vars_elf64(const t_map_data *tmpl, const t_inject_info *inject)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Shdr	*symtab_shdr;
	char		*strtab;
	size_t		inject_vals[N_INJECT_SYMS];

	ehdr = (Elf64_Ehdr *)tmpl->map;
	symtab_shdr = get_symtab_shdr_elf64(ehdr);
	if (!symtab_shdr)
		return (-1);
	strtab = get_strtab_ptr_elf64(ehdr);
	if (!strtab)
		return (-1);
	inject_vals[pl_addr_sym] = inject->addr;
	inject_vals[pl_size_sym] = inject->size;
	return (adjust_syms(ehdr, symtab_shdr, strtab, inject_vals));
}