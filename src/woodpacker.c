//
// Created by Darth Butterwell on 8/7/21.
//
#include "woodpacker.h"
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
	t_exec_data *exec_data;

	if (argc != 2)
		usage();
	exec_data = load_exec(argv[1]);
	if (!exec_data)
		exit(errno);
	if (pack_exec(exec_data))
		exit(errno);
	printf("Provided file is valid and supported!");
	del_exec_data(&exec_data);
    return (0);
}