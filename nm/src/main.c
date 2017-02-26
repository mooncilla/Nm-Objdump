/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sun Feb 26 03:13:09 2017 Full Name
*/

#include      "nm.h"

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

void  read_section(t_core *core, Elf64_Shdr *sh, void *str_ptr, t_list *list)
{
  Elf64_Sym *buff;
  int i;

	i = -1;
  lseek(core->fd, (off_t)sh->sh_offset, SEEK_SET);
  while (++i * (IS_32 ? sizeof(Elf32_Sym)
		: sizeof(Elf64_Sym)) < sh->sh_size)
  {
    buff = malloc(sizeof(Elf64_Sym));
    if (IS_32)
      fill_Sym32(core->fd, buff);
    else
      read(core->fd, buff, sizeof(Elf64_Sym));
    if (strlen((str_ptr + buff->st_name)) > 0 &&
    	(buff->st_shndx == SHN_ABS ?
				(ELF64_ST_BIND(buff->st_info) == STB_LOCAL ? 0 : 1) : 1))
      		add_to_list(list, buff, str_ptr);
    }
}

void print_symbol_table(t_core *core, char *str_ptr,
	t_list *list, char *sh_strtab_p)
{
	while (list)
  {
    if (strlen(str_ptr + list->sym_tbl->st_name) > 0)
    {
	    if (list->sym_tbl->st_shndx != SHN_UNDEF)
      {
        printf("%0*lx ", (IS_32 ? 8	 : 16),
				list->sym_tbl->st_value);
        printf("%c ", get_type(list->sym_tbl, core->sh_tbl, sh_strtab_p));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
      else
      {
      	printf("%*c ", (IS_32 ? 10 : 18),
				get_type(list->sym_tbl, core->sh_tbl, sh_strtab_p));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
    }
    list = list->next;
	}
}

void init_symbol_table(int fd, t_core *core, int symbol_table)
{
	char *str_ptr;
	char *sh_strtab_p;
	int str_tbl_ndx;
  t_list *list;
  Elf64_Shdr *sh_strtab;

  list = malloc(sizeof(t_list));
	list->next = NULL;
	list->sym_tbl = NULL;
  str_tbl_ndx = core->sh_tbl[symbol_table].sh_link;
  str_ptr = malloc(core->sh_tbl[str_tbl_ndx].sh_size);
  lseek(fd, (off_t)core->sh_tbl[str_tbl_ndx].sh_offset, SEEK_SET);
	read(fd, str_ptr, core->sh_tbl[str_tbl_ndx].sh_size);
  read_section(core, &core->sh_tbl[symbol_table], str_ptr, list);
  sh_strtab = &core->sh_tbl[core->eh->e_shstrndx];
  sh_strtab_p = malloc(sh_strtab->sh_size);
  lseek(fd, (off_t)sh_strtab->sh_offset, SEEK_SET);
  read(fd, sh_strtab_p, sh_strtab->sh_size);
	print_symbol_table(core, str_ptr, list, sh_strtab_p);
  free(sh_strtab_p);
	free(str_ptr);
}

void print_symbols(int fd, t_core *core)
{
	int i;

  i = -1;
  while (++i < core->eh->e_shnum)
  {
		if (core->sh_tbl[i].sh_type == SHT_SYMTAB)
      init_symbol_table(fd, core, i);
	 }
}

bool is_ELF(Elf64_Ehdr *eh, char *name, char *file_name)
{
	if (strncmp((char*)eh->e_ident, "\177ELF", 4) == 0)
	return (1);
	printf("%s: %s: File format not recognized\n", name, file_name);
	return (0);
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
