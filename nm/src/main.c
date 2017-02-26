/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sun Feb 26 21:30:24 2017 Full Name
*/

#include      "nm.h"

int is_ELF(Elf64_Ehdr *eh, char *name, char *file_name)
{
	if (strncmp((char*)eh->e_ident, "\177ELF", 4) == 0)
	return (1);
	printf("%s: %s: File format not recognized\n", name, file_name);
	return (0);
}

void			add_to_list(t_list *list, Elf64_Sym *symb, char *str)
{
  t_list *tmp;
  Elf64_Sym *sym_tmp;

  tmp = list;
  if (list->sym_tbl == NULL)
    list->sym_tbl = symb;
	else if (strcoll(str + list->sym_tbl->st_name, str + symb->st_name) > 0)
  {
      sym_tmp = list->sym_tbl;
      tmp = list->next;
      list->sym_tbl = symb;
      list->next = malloc(sizeof(t_list));
      list->next->sym_tbl = sym_tmp;
      list->next->next = tmp;
  }
  else
  {
    while (list->next && strcoll(str + list->next->sym_tbl->st_name,
       str + symb->st_name) <= 0)
      list = list->next;
    tmp = list->next;
    list->next = malloc(sizeof(t_list));
    list->next->sym_tbl = symb;
    list->next->next = tmp;
  }
}

void read_section_header_table(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_tbl)
{
	int i;

  i = -1;
  lseek(fd, eh->e_shoff, SEEK_SET);
  while (++i < eh->e_shnum)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Sh32(fd, i, sh_tbl);
    else
      read(fd, &sh_tbl[i], sizeof(Elf64_Shdr));
  }
}

void launch_nm(int fd, int ac, char *name, char *file_name)
{
	t_core *core;

	core = malloc(sizeof(t_core));
  core->eh = malloc(sizeof(Elf64_Ehdr));
  lseek(fd, (off_t)0, SEEK_SET);
  read(fd, core->eh, sizeof(Elf64_Ehdr));
  if (is_ELF(core->eh, name, (ac == 1) ? "a.out" : file_name) == 1)
  {
    if (IS_32)
      fill_Eh32(core->eh);
    if (lseek(fd, core->eh->e_shoff, SEEK_SET)
      + (core->eh->e_shentsize * core->eh->e_shnum) > lseek(fd, 0, SEEK_END))
      fprintf(stderr, "%s: %s: File truncated\n", name, file_name);
    else
    {
      (ac > 2 ? printf("\n%s:\n", file_name) : 0);
      core->sh_tbl = malloc(sizeof(Elf64_Shdr) * core->eh->e_shnum);
			core->fd = fd;
      read_section_header_table(fd, core->eh, core->sh_tbl);
      print_symbols(fd, core);
      free(core->sh_tbl);
    }
  }
	free(core->eh);
	free(core);
}

int main(int ac, char *av[])
{
  int fd;
  int i;

  setlocale(LC_ALL, "");
  i = (ac == 1 ? -1 : 0);
  while (++i < ac)
  {
    fd = open((ac == 1) ? "a.out" : av[i], O_RDONLY | O_SYNC);
	  if (fd > 0)
      launch_nm(fd, ac, av[0], (ac == 1) ? "a.out" : av[i]);
    else
      printf("%s: '%s': No such file\n", av[0], av[i]);
    }
	return 0;
}
