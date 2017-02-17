/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Fri Feb 17 21:05:14 2017 Full Name
*/

#include      "../includes/nm.h"

char            print_type(Elf64_Sym sym, Elf64_Shdr *shdr)
{
  char  c;

  if (ELF64_ST_BIND(sym.st_info) == STB_GNU_UNIQUE)
    c = 'u';
  else if (ELF32_ST_BIND(sym.st_info) == STB_WEAK)
  {
    c = (ELF32_ST_TYPE(sym.st_info) == STT_OBJECT) ? 'V' : 'W';
    if (sym.st_shndx == SHN_UNDEF)
      c += 32;
  }
  else if (sym.st_shndx == SHN_UNDEF)
    c = 'U';
    else if (sym.st_shndx == SHN_ABS)
    c = 'A';
  else if (sym.st_shndx == SHN_COMMON)
    c = 'C';
  else if (shdr[sym.st_shndx].sh_type == SHT_NOBITS
       && shdr[sym.st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE))
    c = 'B';
  else if (shdr[sym.st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym.st_shndx].sh_flags == SHF_ALLOC)
    c = 'R';
  else if (shdr[sym.st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym.st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE))
    c = 'D';
  else if (shdr[sym.st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym.st_shndx].sh_flags == (SHF_ALLOC | SHF_EXECINSTR))
    c = 'T';
  else if (shdr[sym.st_shndx].sh_type == SHT_DYNAMIC)
    c = 'D';
  else
    c = '?';
  if (ELF64_ST_BIND(sym.st_info) == STB_LOCAL && c != '?')
    c += 32;
  return c;
}

bool is_ELF(Elf64_Ehdr eh)
{
	if(!strncmp((char*)eh.e_ident, "\177ELF", 4))
		return (1);
  else
		return (0);
}

void read_section_header_table(int fd, Elf64_Ehdr eh, Elf64_Shdr *sh_table)
{
	int i;

  i = -1;
	lseek(fd, (off_t)eh.e_shoff, SEEK_SET);
  while (++i < eh.e_shnum)
  	read(fd, &sh_table[i], eh.e_shentsize);
}

int				compare_str(char *str1, char *str2)
{
  int		i;
  char  *str1_tmp;
  char  *str2_tmp;

  char *test1;
  char *test2;

  i = 0;
  str1_tmp = strdup(str1);
  str2_tmp = strdup(str2);
  test1 = str1_tmp;
  test2 = str2_tmp;

  while (test1[0] == '_')
    test1++;
  while (test2[0] == '_')
    test2++;
  while (test1[i] && test2[i])
  {
    (test1[i] >= 'A' && test1[i] <= 'Z' ? test1[i] += 32 : 0);
    (test2[i] >= 'A' && test2[i] <= 'Z' ? test2[i] += 32 : 0);
    i++;
  }
  //printf("value=%s %s\n", test1, test2);
  i = strcmp(test1, test2);
  free(str1_tmp);
  free(str2_tmp);
  return (i);
}

void			add_to_list(t_list *list, Elf64_Sym *symb, void *str_ptr)
{
  t_list *tmp;

  tmp = list;
  if (list->sym_tbl == NULL)
    list->sym_tbl = symb;
  /*else if (compare_str(str_ptr + list->sym_tbl->st_name,
       str_ptr + symb->st_name) > 0)
  {
      Elf64_Sym *sym_tmp = list->sym_tbl;

      tmp = list->next;
      list->sym_tbl = symb;
      list->next = malloc(sizeof(t_list));
      list->next->sym_tbl = sym_tmp;
      list->next->next = tmp;
  }*/
  else
  {
    while (list->next && compare_str(str_ptr + list->next->sym_tbl->st_name,
       str_ptr + symb->st_name) < 0)
      list = list->next;
    tmp = list->next;
    list->next = malloc(sizeof(t_list));
    list->next->sym_tbl = symb;
    list->next->next = tmp;
    //compare_str(str_ptr + list->sym_tbl->st_name, str_ptr + symb->st_name);
  }
  //printf("INDEX = %016x SYMB = %s\n", index, str_ptr + symb->st_name);
}

Elf64_Sym *read_section(int fd, Elf64_Shdr sh, void *str_ptr, t_list *list)
{
	//char* buff = malloc(sh.sh_size);
  Elf64_Sym *buff;
  int i;
  i = -1;

  lseek(fd, (off_t)sh.sh_offset, SEEK_SET);
  while (++i * sizeof(Elf64_Sym) < sh.sh_size)
  {
    buff = malloc(sizeof(Elf64_Sym));
    read(fd, buff, sizeof(Elf64_Sym));
    //printf("%016lx %s\n", buff->st_value, str_ptr + buff->st_name);
    if (strlen((str_ptr + buff->st_name)) > 0 && buff->st_shndx != SHN_ABS)
      add_to_list(list, buff, str_ptr);
  }
	 return (buff);
}

void print_symbol_table(int fd,	Elf64_Ehdr *eh, Elf64_Shdr *sh_table, int symbol_table)
{
	void *str_ptr;
	Elf64_Sym *sym_tbl;
  int symbol_count;
  int str_tbl_ndx;
  t_list *list;

  list = malloc(sizeof(t_list)); list->next = NULL; list->sym_tbl = NULL;
  str_tbl_ndx = sh_table[symbol_table].sh_link;
//  str_ptr = (void *) read_section(fd, sh_table[str_tbl_ndx]);
  str_ptr = malloc(sh_table[str_tbl_ndx].sh_size);
  lseek(fd, (off_t)sh_table[str_tbl_ndx].sh_offset, SEEK_SET);
	read(fd, str_ptr, sh_table[str_tbl_ndx].sh_size);

  symbol_count = (sh_table[symbol_table].sh_size / sizeof(Elf64_Sym));

  sym_tbl = read_section(fd, sh_table[symbol_table], str_ptr, list);


//char            print_type(Elf64_Sym sym, Elf64_Shdr *shdr);
	while (list)
  {
    if (strlen(str_ptr + list->sym_tbl->st_name) > 0)
    {
	    if (list->sym_tbl->st_value > 0)
      {
        printf("%016lx ", list->sym_tbl->st_value);
        printf("%c ", print_type(*list->sym_tbl, sh_table));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
      else
      {
        printf("%18c ", print_type(*list->sym_tbl, sh_table));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
    }
    list = list->next;
	}
}

void print_symbols(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;

	for(i = 0; i < eh->e_shnum; i++)
  {
		if ((sh_table[i].sh_type == SHT_SYMTAB)/*
        || (sh_table[i].sh_type == SHT_DYNSYM)*/)
    {
      print_symbol_table(fd, eh, sh_table, i);
			//printf("\n[Section %03d]", i);
		}
	}
}

int main(int ac, char *av[])
{
	int fd;
	Elf64_Ehdr *eh;
	Elf64_Shdr *sh_tbl;

	if (ac != 2)
		return (0);
	fd = open(av[1], O_RDONLY|O_SYNC);
	if (fd < 0)
  {
		printf("Unable to open %s\n", av[1]);
		return (0);
	}
  eh = malloc(sizeof(Elf64_Ehdr));
  lseek(fd, (off_t)0, SEEK_SET);
  read(fd, (Elf64_Ehdr *)eh, sizeof(Elf64_Ehdr));
  sh_tbl = malloc(eh->e_shentsize * eh->e_shnum);
	read_section_header_table(fd, *eh, sh_tbl);

	print_symbols(fd, eh, sh_tbl);
	return 0;
}
