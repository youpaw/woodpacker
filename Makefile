# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbutterw <dbutterw@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/06/09 21:25:19 by dbutterw          #+#    #+#              #
#    Updated: 2021/06/09 21:25:19 by dbutterw         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

LIBFT_DIR   := ./libft

INC_DIRS	:=	src/. src/load src/crypt src/pack src/pack/elf64 src/pack/elf32 \
				src/hash src/compress
INC_FLAGS	:=	$(addprefix -I ,$(INC_DIRS)) -I $(LIBFT_DIR)

SRCS_C		:=  woodpacker.c \
                new_data_wrap.c \
                del_data_wrap.c \
                crypt/generate_key.c \
                compress/compress.c \
                load/load_exec.c \
                load/mmap_exec.c \
                load/munmap_exec.c \
                load/validate_exec.c \
                load/validate_exec_elf32.c \
                load/validate_exec_elf64.c \
                pack/pack_exec.c \
                pack/write_woody.c \
                pack/allocate_woody.c \
                pack/elf32/pack_elf32.c \
                pack/elf32/find_cave_elf32.c \
                pack/elf32/assemble_woody_elf32.c \
                pack/elf64/pack_elf64.c \
                pack/elf64/find_cave_elf64.c \
                pack/elf64/assemble_woody_elf64.c \
                pack/del_cave_info.c

SRCS_ASM	:= crypt/encrypt.asm

OBJ_DIR		:= ./obj
OBJ_DIRS	:= $(addprefix $(OBJ_DIR)/,$(INC_DIRS:src/%=%))

OBJS_C		:= $(addprefix $(OBJ_DIR)/,$(SRCS_C:.c=.o))
OBJS_ASM    := $(addprefix $(OBJ_DIR)/,$(SRCS_ASM:.asm=.s.o))

DEPS    	:= $(OBJS_C:.o=.d) $(OBJS_ASM:.s.o=.d)

LIBFT		:= $(LIBFT_DIR)/libft.a
CFLAGS		:= -MMD -Wall -Wextra
LFLAGS		:= -L $(LIBFT_DIR) -lft
NAME		:= woody_woodpacker

.PHONY: all clean fclean re

all: $(NAME)

$(OBJ_DIRS):
	mkdir -p $(OBJ_DIRS)

$(OBJ_DIR)/%.o: src/%.c
	gcc $(CFLAGS) $(INC_FLAGS) -I . -o $@ -c $<

$(OBJ_DIR)/%.s.o: src/%.asm
	nasm -f elf64 -o $@ $<

-include $(DEPS)

$(NAME): $(OBJ_DIRS) $(OBJS_C) $(OBJS_ASM) | lib
	gcc -o $(NAME) $(OBJS_C) $(OBJS_ASM) $(LFLAGS)

lib:
	make -C $(LIBFT_DIR)
clean:
	rm -rf $(OBJ_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBFT_DIR)

re: fclean all
