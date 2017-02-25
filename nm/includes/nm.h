/*
** nm.h for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 23:30:28 2017 Full Name
** Last update	Sat Feb 25 19:46:53 2017 Full Name
*/

#ifndef _NM_H_
# define _NM_H_

#include <locale.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <elf.h>

#define IS_32 core->eh->e_ident[EI_CLASS] == ELFCLASS32

typedef struct  s_list
{
  Elf64_Sym     *sym_tbl;
  struct s_list *next;
}               t_list;

typedef struct  s_core
{
  Elf64_Ehdr    *eh;
  Elf64_Shdr    *sh_tbl;
  int           fd;
}               t_core;

#endif /* !_NM_H_ */
