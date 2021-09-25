//
// Created by youpaw on 11/09/2021.
//

#include <stdio.h>
size_t payload_size = 0xF;
size_t payload_addr = 0xF;

int main(void)
{
	printf("size=%zd,addr=%zd!\n", payload_size, payload_addr);
	return (0);
}