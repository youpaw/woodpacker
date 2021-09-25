//
// Created by youpaw on 27/08/2021.
//

#ifndef PACK_EXEC_H
#define PACK_EXEC_H
#define WOODY_FILE "woody"
#define PAYLOAD_ADDR_SYM "payload_addr"
#define PAYLOAD_SIZE_SYM "payload_size"
#define N_INJECT_SYMS 2
#include "load_exec.h"

typedef struct s_map_data {
	void	*map;
	size_t	size;
}				t_map_data;

typedef struct s_inject_info {
	size_t	file_off;
	size_t	addr;
	size_t	size;
}				t_inject_info;

typedef enum e_inject_sym{
	pl_addr_sym,
	pl_size_sym
}				t_inject_sym;

int		pack_exec(const t_exec_data *exec_data);
void	get_map_data(t_map_data *data, void *map, size_t size);

#endif //PACK_EXEC_H
