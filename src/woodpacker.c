//
// Created by Darth Butterwell on 8/7/21.
//
#include "load_exec.h"
#include "crypt.h"
#include "pack_exec.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

static void usage(void)
{
	puts("Usage: woody_woodpacker executable\n");
	puts("\tEncrypt and pack executable\n");
	exit(0);
}

int main(int argc, const char **argv)
{
	t_exec_map	*exec;
	t_data_wrap	*key;

	if (argc != 2)
		usage();
	exec = load_exec(argv[1]);
	if (!exec)
		exit(errno);
	key = generate_key(NULL);
	if (pack_exec(exec, key))
		exit(errno);
	munmap_exec(&exec);
	del_data_wrap(&key);
    return (0);
}
