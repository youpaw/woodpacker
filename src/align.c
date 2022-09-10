//
// Created by youpaw on 9/10/22.
//
#include <stddef.h>

size_t align(size_t insert, size_t current, size_t align)
{
	return (((insert / align) + (insert % align > current)) * align);
}