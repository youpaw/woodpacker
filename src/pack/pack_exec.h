//
// Created by youpaw on 27/08/2021.
//

#ifndef PACK_EXEC_H
#define PACK_EXEC_H
#define WOODY_FILE "woody"
#include "woodpacker.h"
#include "load_exec.h"

/*
 * t_cave_info description
 * off - cave offset inside executable
 * size - cave size
 * pad - exec segment code padding for encryption
 * extend - sizeof of executable extension if payload doesn't fit
 * ph_off - text segment header offset
 * ph_idx - text segment header index
 */
typedef struct	s_cave_info {
	size_t off;
	size_t size;
	size_t extend;
	size_t seg_pad;
	size_t ph_off;
	size_t ph_idx;
}				t_cave_info;

int			pack_exec(const t_exec_map *exec_map, const t_data_wrap *key);
int			write_woody(const t_data_wrap *woody);
#endif //PACK_EXEC_H
