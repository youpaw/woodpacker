//
// Created by youpaw on 8/29/22.
//

#include "pack_elf64.h"
#include "crypt.h"
#include "compress.h"
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
		   "in the provided elf64 executable");
	*txt_idx = -1;
	return (NULL);
}

t_cave_info	*find_cave_elf64(const void *bin)
{
	t_cave_info	*cave;
	Elf64_Phdr	*txt_seg;
	int			txt_seg_idx;
	size_t		tmp;

	txt_seg = find_text_segment(&txt_seg_idx, bin);
	if (!txt_seg)
		return (NULL);
	cave = ft_xmalloc(sizeof(t_cave_info));
	cave->ph_off = (void *) txt_seg - bin;
	cave->ph_idx = txt_seg_idx;
	cave->seg_lz4 = ft_xmalloc(txt_seg->p_filesz);
	cave->size_lz4 = compress(bin + txt_seg->p_offset, cave->seg_lz4, txt_seg->p_filesz);
	cave->off = txt_seg->p_offset + txt_seg->p_filesz;
	cave->size = txt_seg->p_align - txt_seg->p_filesz % txt_seg->p_align;
	cave->seg_off = txt_seg->p_offset;
	cave->enc_pad = cave->size_lz4 % ENCRYPT_ALIGN ?
			ENCRYPT_ALIGN - (cave->size_lz4 % ENCRYPT_ALIGN) : 0;
	tmp = cave->size_lz4 + cave->enc_pad + PAYLOAD_SIZE_ELF64;
	cave->extra = tmp > txt_seg->p_filesz ?
				  tmp - txt_seg->p_filesz + LOAD_SIZE_ELF64 : LOAD_SIZE_ELF64;
	cave->extend = ALIGN_SEG(cave->extra, cave->size, txt_seg->p_align);
	cave->ld_pad = tmp < txt_seg->p_filesz ? txt_seg->p_filesz - tmp : 0;
	return (cave);
}
