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
	t_exec_map *exec;

	if (argc != 2)
		usage();
	exec = load_exec(argv[1]);
	if (!exec)
		exit(errno);
	if (pack_exec(exec))
		exit(errno);
	printf("Provided file is valid and supported!");
	munmap_exec(&exec);
    return (0);
}