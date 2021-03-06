/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sun Feb 26 21:32:08 2017 Full Name
*/

#include	"objdump.h"

int		is_ELF(Elf64_Ehdr *eh, char *name, char *file_name)
{
  if (strncmp((char*)eh->e_ident, "\177ELF", 4) == 0)
    return (1);
  printf("%s: %s: File format not recognized\n", name, file_name);
  return (0);
}

void		read_section_header_table(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_tbl)
{
  int		i;

  i = -1;
  lseek(fd, (off_t)eh->e_shoff, SEEK_SET);
  while (++i < eh->e_shnum)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Sh32(fd, i, sh_tbl);
    else
      read(fd, &sh_tbl[i], sizeof(Elf64_Shdr));
  }
}

void		launch_objdump(int fd, int ac, char *name, char *file_name)
{
  Elf64_Ehdr	*eh;
  Elf64_Shdr	*sh_tbl;

  eh = malloc(sizeof(Elf64_Ehdr));
  lseek(fd, (off_t)0, SEEK_SET);
  read(fd, (Elf64_Ehdr *)eh, sizeof(Elf64_Ehdr));
  if (is_ELF(eh, name, (ac == 1) ? "a.out" : file_name) == 1)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Eh32(eh);
    if (lseek(fd, eh->e_shoff, SEEK_SET)
     + (eh->e_shentsize * eh->e_shnum) > lseek(fd, 0, SEEK_END))
      fprintf(stderr, "%s: %s: File truncated\n", name, file_name);
    else
    {
      sh_tbl = malloc(sizeof(Elf64_Shdr) * eh->e_shnum);
      read_section_header_table(fd, eh, sh_tbl);
      print_header(eh, sh_tbl,(ac == 1) ? "a.out" : file_name);
      print_section_tables(fd, eh, sh_tbl, &sh_tbl[eh->e_shstrndx]);
      free(eh);
      free(sh_tbl);
    }
  }
}

int		main(int ac, char *av[])
{
  int		fd;
  int		i;

  i = (ac == 1 ? -1 : 0);
  while (++i < ac)
  {
    fd = open((ac == 1) ? "a.out" : av[i], O_RDONLY | O_SYNC);
    if (fd > 0)
      launch_objdump(fd, ac, av[0], (ac == 1) ? "a.out" : av[i]);
    else
      printf("%s: '%s': No such file\n", av[0], av[i]);
  }
  return 0;
}
