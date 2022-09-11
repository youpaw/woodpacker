//
// Created by youpaw on 8/29/22.
//

#include "pack_elf64.h"
#include "crypt.h"
#include "memory/ft_mem.h"
#include <errno.h>
#include <stdio.h>
#include <elf.h>

static Elf64_Phdr *find_text_segment(int *txt_idx, const Elf64_Ehdr *ehdr)
{
	Elf64_Phdr	*seg;
	int			idx;

	idx = 0;
	while (idx != ehdr->e_phnum)
	{
		seg = (void *) ehdr + ehdr->e_phoff + ehdr->e_phentsize * idx;
		if (seg->p_type == PT_LOAD && (seg->p_flags & PF_X))
		{
			*txt_idx = idx;
			return (seg);
		}
		idx++;
	}
	errno = EINVAL;
	perror("Cannot find executable segment header "
		   "in provided elf64 executable");
	*txt_idx = -1;
	return (NULL);
}

t_cave_info	*find_cave_elf64(const void *bin)
{
	t_cave_info	*cave;
	Elf64_Phdr	*txt_seg;
	int			txt_seg_idx;

	txt_seg = find_text_segment(&txt_seg_idx, bin);
	if (!txt_seg)
		return (NULL);
	cave = ft_xmalloc(sizeof(t_cave_info));
	cave->off = txt_seg->p_offset + txt_seg->p_filesz;
	cave->ph_off = (void *) txt_seg - bin;
	cave->ph_idx = txt_seg_idx;
	cave->size = txt_seg->p_align - txt_seg->p_filesz % txt_seg->p_align;
	cave->seg_pad = txt_seg->p_filesz % ENCRYPT_ALIGN;
	cave->extend = align(cave->seg_pad + PAYLOAD_SIZE_ELF64,
						 cave->size, txt_seg->p_align);
	return (cave);
}