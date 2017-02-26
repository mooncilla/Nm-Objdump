/*
** nm.h for objdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 23:30:28 2017 Full Name
** Last update	Sun Feb 26 21:25:35 2017 Full Name
*/

#ifndef _OBJDUMP_H_
# define _OBJDUMP_H_

#include <locale.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <elf.h>
#include <ctype.h>

#define BFD_NO_FLAGS 0x00
#define HAS_RELOC 0x01
#define EXEC_P 0x02
#define HAS_LINENO 0x04
#define HAS_DEBUG 0x08
#define HAS_SYMS 0x10
#define HAS_LOCALS 0x20
#define DYNAMIC 0x40
#define WP_TEXT 0x80
#define D_PAGED 0x100

int	print_header(Elf64_Ehdr *eh, Elf64_Shdr *sh_tbl, char *name);
void					fill_Eh32(Elf64_Ehdr *eh);
void          fill_Sh32(int fd, int i, Elf64_Shdr *sh_tbl);
void          print_section_tables(int fd, Elf64_Ehdr *eh,
  	Elf64_Shdr *sh_tbl, Elf64_Shdr *sh_strtab);

#endif /* !_OBJDUMP_H_ */
