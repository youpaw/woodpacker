//
// Created by youpaw on 11/1/23.
//
#include "pack_elf32.h"
#include "crypt.h"
#include "compress.h"
#include "memory/ft_mem.h"
#include <errno.h>
#include <stdio.h>
#include <elf.h>

static Elf32_Phdr *find_text_segment(int *txt_idx, const Elf32_Ehdr *ehdr)
{
	Elf32_Phdr	*seg;
	int			idx;

	idx = 0;
	while (idx != ehdr->e_phnum)
	{
		seg = (void *) ehdr + ehdr->e_phoff + ehdr->e_phentsize * idx;
		if (seg->p_type == PT_LOAD && (seg->p_flags & PF_X))
		{
			if (seg->p_offset < ehdr->e_phoff + \
				ehdr->e_phentsize * ehdr->e_phnum)
				break;
			*txt_idx = idx;
			return (seg);
		}
		idx++;
	}
	errno = EINVAL;
	if (idx == ehdr->e_phnum)
		perror("Cannot find executable segment header"
			   "in the provided elf32 executable");
	else
		perror("Executable segment header is invalid");
	*txt_idx = -1;
	return (NULL);
}

t_cave_info	*find_cave_elf32(const void *bin)
{
	t_cave_info	*cave;
	Elf32_Phdr	*txt_seg;
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
	tmp = cave->size_lz4 + cave->enc_pad + PAYLOAD_SIZE_ELF32;
	cave->extra = tmp > txt_seg->p_filesz ?
				  tmp - txt_seg->p_filesz + LOAD_SIZE_ELF32 : LOAD_SIZE_ELF32;
	cave->extend = ALIGN_SEG(cave->extra, cave->size, txt_seg->p_align);
	cave->ld_pad = tmp < txt_seg->p_filesz ? txt_seg->p_filesz - tmp : 0;
	return (cave);
}