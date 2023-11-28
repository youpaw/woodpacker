//
// Created by youpaw on 11/1/23.
//

#ifndef PACK_ELF32_H
#define PACK_ELF32_H

#include "pack_exec.h"
#include <unistd.h>

// The key is placed just before the payload
#define PAYLOAD_ELF32 "\x50\x57\x56\x52\x51\xE8\x0F\x00\x00\x00\xE8\x67\x00\x00\x00\x59\x5A\x5E\x5F\x58\xE9\xEB\xBE\xAD\xDE\x55\x89\xE5\x83\xEC\x10\xC7\x45\xFC\x2E\x0A\x00\x00\xC7\x45\xF8\x79\x2E\x2E\x2E\xC7\x45\xF4\x57\x6F\x6F\x64\xC7\x45\xF0\x2E\x2E\x2E\x2E\xB8\x04\x00\x00\x00\xBB\x01\x00\x00\x00\x8D\x4D\xF0\xBA\x0E\x00\x00\x00\xCD\x80\xC9\xC3\x66\x0F\x70\xD2\xFF\xC5\xC1\x73\xF9\x04\x66\x0F\xEF\xCF\xC5\xC1\x73\xF9\x04\x66\x0F\xEF\xCF\xC5\xC1\x73\xF9\x04\x66\x0F\xEF\xCF\x66\x0F\xEF\xCA\xC3\x55\x89\xE5\x83\xEC\x18\xC7\x45\xFC\xEF\xBE\xAD\xDE\xC7\x45\xF4\xEF\xBE\xAD\xDE\xC7\x45\xF0\xEF\xBE\xAD\xDE\xC7\x45\xEC\xEF\xBE\xAD\xDE\xC7\x45\xE8\xEF\xBE\xAD\xDE\x0F\x10\x4D\xE8\x8B\x75\xFC\xF7\xDE\x01\xF7\x89\x7D\xF8\x89\xFE\x81\xE7\x00\xF0\xFF\xFF\xF7\xDF\x01\xFE\xF7\xDF\x03\x75\xFC\xB8\x7D\x00\x00\x00\x89\xFB\x89\xF1\xBA\x07\x00\x00\x00\xCD\x80\x8B\x7D\xF8\x8B\x75\xFC\x0F\x28\xC1\x66\x0F\x3A\xDF\xD1\x01\xE8\x6F\xFF\xFF\xFF\x0F\x28\xD9\x66\x0F\x3A\xDF\xD1\x02\xE8\x61\xFF\xFF\xFF\x0F\x28\xE1\x66\x0F\x3A\xDF\xD1\x04\xE8\x53\xFF\xFF\xFF\x0F\x28\xE9\x66\x0F\x3A\xDF\xD1\x08\xE8\x45\xFF\xFF\xFF\x0F\x28\xF1\x66\x0F\x3A\xDF\xD1\x10\xE8\x37\xFF\xFF\xFF\x0F\x28\xF9\x66\x0F\x38\xDB\xDB\x66\x0F\x38\xDB\xE4\x66\x0F\x38\xDB\xED\x66\x0F\x38\xDB\xF6\x31\xD2\x39\xD6\x7E\x2A\x0F\x10\x0C\x17\x66\x0F\xEF\xCF\x66\x0F\x38\xDE\xCE\x66\x0F\x38\xDE\xCD\x66\x0F\x38\xDE\xCC\x66\x0F\x38\xDE\xCB\x66\x0F\x38\xDF\xC8\x0F\x11\x0C\x17\x83\xC2\x10\xEB\xD2\xC9\xC3"
#define PAYLOAD_SIZE_ELF32 355

#define KEY_PART1_OFF_ELF32 155
#define KEY_PART2_OFF_ELF32 148
#define KEY_PART3_OFF_ELF32 141
#define KEY_PART4_OFF_ELF32 134
#define SEG_LENGTH_OFF_ELF32 127
#define JMP_ADDR_OFF_ELF32 21
#define ALIGN_OFF_ELF32 177

t_data_wrap *pack_elf32(const t_exec_map *exec, const t_data_wrap *key);
t_cave_info	*find_cave_elf32(const void *bin);
void		assemble_woody_elf32(t_data_wrap *woody, t_cave_info *cave_info,
								 const t_data_wrap *key);

#endif //PACK_ELF32_H
