/*
** nm.h for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 23:30:28 2017 Full Name
** Last update	Sun Feb 26 21:29:40 2017 Full Name
*/

#ifndef _NM_H_
# define _NM_H_

#include <locale.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

char         get_type(Elf64_Sym *sym, Elf64_Shdr *shdr, char *sh_strtab_p);
void					fill_Sym32(int fd, Elf64_Sym *buff);
void          fill_Sh32(int fd, int i, Elf64_Shdr *sh_tbl);
void					fill_Eh32(Elf64_Ehdr *eh);
void print_symbols(int fd, t_core *core);
void			add_to_list(t_list *list, Elf64_Sym *symb, char *str);

#endif /* !_NM_H_ */
