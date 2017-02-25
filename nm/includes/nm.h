/*
** nm.h for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 23:30:28 2017 Full Name
** Last update	Sat Feb 25 00:23:21 2017 Full Name
*/

#ifndef _NM_H_
# define _NM_H_
#include <locale.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include  <sys/mman.h>
#include  <sys/stat.h>
#include <elf.h>
#include <dlfcn.h>

typedef struct s_list
{
  Elf64_Sym *sym_tbl;
  struct s_list *next;
}              t_list;

#endif /* !_NM_H_ */
