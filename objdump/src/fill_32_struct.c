/*
** fill_32_struct.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Tue Feb 21 12:38:41 2017 Full Name
** Last update	Fri Feb 24 01:44:36 2017 Full Name
*/

#include      "objdump.h"

void          fill_Sh32(int fd, int i, Elf64_Shdr *sh_tbl)
{
  Elf32_Shdr *sh32;

  sh32 = malloc(sizeof(Elf32_Shdr));
  read(fd, sh32, sizeof(Elf32_Shdr));
  sh_tbl[i].sh_name      = sh32->sh_name;
  sh_tbl[i].sh_type      = sh32->sh_type;
  sh_tbl[i].sh_flags     = sh32->sh_flags;
  sh_tbl[i].sh_addr      = (Elf64_Addr) sh32->sh_addr;
  sh_tbl[i].sh_offset    = (Elf64_Off) sh32->sh_offset;
  sh_tbl[i].sh_size      = (uint64_t) sh32->sh_size;
  sh_tbl[i].sh_link      = sh32->sh_link;
  sh_tbl[i].sh_info      = sh32->sh_info;
  sh_tbl[i].sh_addralign = (uint64_t) sh32->sh_addralign;
  sh_tbl[i].sh_entsize   = (uint64_t) sh32->sh_entsize;
  free(sh32);
}

void					fill_Eh32(Elf64_Ehdr *eh, Elf32_Ehdr eh32)
{
  eh->e_type      = eh32.e_type;
  eh->e_machine   = eh32.e_machine;
  eh->e_version   = eh32.e_version;
  eh->e_entry     = (Elf64_Addr) eh32.e_entry;
  eh->e_phoff     = (Elf64_Off) eh32.e_phoff;
  eh->e_shoff     = (Elf64_Off) eh32.e_shoff;
  eh->e_flags     = eh32.e_flags;
  eh->e_ehsize    = eh32.e_ehsize;
  eh->e_phentsize = eh32.e_phentsize;
  eh->e_phnum     = eh32.e_phnum;
  eh->e_shentsize = eh32.e_shentsize;
  eh->e_shnum     = eh32.e_shnum;
  eh->e_shstrndx  = eh32.e_shstrndx;
}
