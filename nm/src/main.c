  /*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sat Feb 18 00:33:15 2017 Full Name
*/

#include      "nm.h"

bool is_ELF(Elf64_Ehdr *eh, char *name, char *file_name)
{
	if(strncmp((char*)eh->e_ident, "\177ELF", 4) == 0)
		return (1);
  printf("%s: %s: File format not recognized\n", name, file_name);
	return (0);
}

void read_section_header_table(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;

  i = -1;
	lseek(fd, (off_t)eh->e_shoff, SEEK_SET);
  while (++i < eh->e_shnum)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Sh32(fd, i, sh_table);
    else
      read(fd, &sh_table[i], sizeof(Elf64_Shdr));
  }
}

void			add_to_list(t_list *list, Elf64_Sym *symb, char *str_ptr)
{
  t_list *tmp;
  Elf64_Sym *sym_tmp;

  tmp = list;
  if (list->sym_tbl == NULL)
    list->sym_tbl = symb;
  else if (strcoll(str_ptr + list->sym_tbl->st_name,
    str_ptr + symb->st_name) > 0)
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
    while (list->next && strcoll(str_ptr + list->next->sym_tbl->st_name,
       str_ptr + symb->st_name) <= 0)
      list = list->next;
    tmp = list->next;
    list->next = malloc(sizeof(t_list));
    list->next->sym_tbl = symb;
    list->next->next = tmp;
  }
}

void  read_section(Elf64_Ehdr *eh, int fd, Elf64_Shdr sh, void *str_ptr, t_list *list)
{
  Elf64_Sym *buff;
  int i;
  i = -1;

  lseek(fd, (off_t)sh.sh_offset, SEEK_SET);

  while (++i * (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
     sizeof(Elf32_Sym) : sizeof(Elf64_Sym)) < sh.sh_size)
  {
    buff = malloc(sizeof(Elf64_Sym));
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Sym32(fd, buff);
    else
      read(fd, buff, sizeof(Elf64_Sym));
    if (strlen((str_ptr + buff->st_name)) > 0 &&
    (buff->st_shndx == SHN_ABS ? (ELF64_ST_BIND(buff->st_info) == STB_LOCAL ? 0 : 1) : 1))
      add_to_list(list, buff, str_ptr);
    }
}

void print_symbol_table(int fd,	Elf64_Ehdr *eh, Elf64_Shdr *sh_table, int symbol_table)
{
	char *str_ptr;
	int str_tbl_ndx;
  t_list *list;
  Elf64_Shdr *sh_strtab;

  list = malloc(sizeof(t_list)); list->next = NULL; list->sym_tbl = NULL;
  str_tbl_ndx = sh_table[symbol_table].sh_link;
  str_ptr = malloc(sh_table[str_tbl_ndx].sh_size);
  lseek(fd, (off_t)sh_table[str_tbl_ndx].sh_offset, SEEK_SET);
	read(fd, str_ptr, sh_table[str_tbl_ndx].sh_size);
  read_section(eh, fd, sh_table[symbol_table], str_ptr, list);


  sh_strtab = &sh_table[eh->e_shstrndx];
  char *sh_strtab_p = malloc(sh_strtab->sh_size);
  lseek(fd, (off_t)sh_strtab->sh_offset, SEEK_SET);
  read(fd, sh_strtab_p, sh_strtab->sh_size);


	while (list)
  {
    if (strlen(str_ptr + list->sym_tbl->st_name) > 0)
    {
	    if (list->sym_tbl->st_shndx != SHN_UNDEF)
      {
        (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
          printf("%08lx ", list->sym_tbl->st_value) :
          printf("%016lx ", list->sym_tbl->st_value));
          printf("%c ", print_type(list->sym_tbl, sh_table, sh_strtab_p));
          printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
      else
      {
        (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
          printf("%10c ", print_type(list->sym_tbl, sh_table, sh_strtab_p)) :
            printf("%18c ", print_type(list->sym_tbl, sh_table, sh_strtab_p)));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
    }
    list = list->next;
	}
  free(sh_strtab_p);
}

void print_symbols(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;

  i = -1;
	while (++i < eh->e_shnum)
  {
		if (sh_table[i].sh_type == SHT_SYMTAB)
      print_symbol_table(fd, eh, sh_table, i);
	 }
}

int main(int ac, char *av[])
{
	int fd;
	Elf64_Ehdr *eh;
	Elf64_Shdr *sh_tbl;
  int i;

  i = (ac == 1 ? -1 : 0);
  while (++i < ac)
  {
    fd = open((ac == 1) ? "a.out" : av[i], O_RDONLY | O_SYNC);
	  if (fd > 0)
    {
      setlocale(LC_ALL, "");
      eh = malloc(sizeof(Elf64_Ehdr));
      lseek(fd, (off_t)0, SEEK_SET);
      read(fd, (Elf64_Ehdr *)eh, sizeof(Elf64_Ehdr));
      if (is_ELF(eh, av[0], av[1]) == 1)
      {
        (ac > 2 ? printf("\n%s:\n", av[i]) : 0);
        if (eh->e_ident[EI_CLASS] == ELFCLASS32)
          fill_Eh32(eh, *eh);
        sh_tbl = malloc(sizeof(Elf64_Shdr) * eh->e_shnum);
        read_section_header_table(fd, eh, sh_tbl);
        print_symbols(fd, eh, sh_tbl);
      }
    }
    else
      printf("%s: '%s': No such file\n", av[0], av[i]);
    }
	return 0;
}
