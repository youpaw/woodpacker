//
// Created by youpaw on 27/08/2021.
//

#ifndef PACK_EXEC_H
#define PACK_EXEC_H
#define WOODY_FILE "woody"
#include "woodpacker.h"
#include "load_exec.h"

#define ALIGN_SEG(insert, current, align) (						\
		((insert / align) + (insert % align > current)) * align \
)

/*
 * t_cave_info description
 * off - cave offset inside executable
 * size - cave size
 * pad - exec segment code padding for encryption
 * extend - sizeof of executable extension if payload doesn't fit (aligned)
 * ph_off - text segment header offset
 * ph_idx - text segment header index
 */
typedef struct	s_cave_info {
	size_t	off;
	size_t	size;
	size_t	extra;
	size_t	extend;
	size_t	seg_off;
	size_t	ph_off;
	size_t	ph_idx;
	size_t	enc_pad;
	size_t	ld_pad;
	void	*seg_lz4;
	size_t	size_lz4;
}				t_cave_info;

int			pack_exec(const t_exec_map *exec_map, const t_data_wrap *key);
t_data_wrap	*allocate_woody(const t_exec_map *exec, const t_cave_info *cave);
int			write_woody(const t_data_wrap *woody);
void		del_cave_info(t_cave_info **cave_info);
#endif //PACK_EXEC_H
