  /*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sat Feb 18 00:33:15 2017 Full Name
*/

#include      "../includes/nm.h"

char            print_type(Elf64_Sym *sym, Elf64_Shdr *shdr)
{
  char  c;

  /*if (shdr[sym->st_shndx].sh_type == SHT_PROGBITS)
    printf("index= %d %d ", shdr[sym->st_shndx].sh_type, sym->st_shndx); */
  if (ELF64_ST_BIND(sym->st_info) == STB_GNU_UNIQUE)
    c = 'u';
  else if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
  {
    c = (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT) ? 'V' : 'W';
    if (sym->st_shndx == SHN_UNDEF)
      c += 32;
  }
  else if (sym->st_shndx == SHN_UNDEF)
    c = 'U';
  else if (sym->st_shndx == SHT_SHLIB)
    c = 'N';
  else if (sym->st_shndx == SHN_ABS)
    c = 'A';
  else if (sym->st_shndx == SHN_COMMON)
    c = 'C';
  else if (shdr[sym->st_shndx].sh_type == SHT_NOBITS
       && shdr[sym->st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE))
    c = 'B';
  else if (shdr[sym->st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym->st_shndx].sh_flags == SHF_ALLOC)
    c = 'R';
  else if (shdr[sym->st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym->st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE))
    c = 'D';
  else if (shdr[sym->st_shndx].sh_type == SHT_PROGBITS
       && shdr[sym->st_shndx].sh_flags == (SHF_ALLOC | SHF_EXECINSTR))
    c = 'T';
  else if (shdr[sym->st_shndx].sh_type == SHT_DYNAMIC)
    c = 'D';
  else
    c = '?';
  if (ELF32_ST_BIND(sym->st_info) == STB_LOCAL && c != '?')
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

void read_section_header_table(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;
  Elf32_Shdr *sh32;

  i = -1;
	lseek(fd, (off_t)eh->e_shoff, SEEK_SET);
  sh32 = malloc(sizeof(Elf32_Shdr));
  while (++i < eh->e_shnum)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
    {
      read(fd, sh32, sizeof(Elf32_Shdr));
      sh_table[i].sh_name      = sh32->sh_name;
      sh_table[i].sh_type      = sh32->sh_type;
      sh_table[i].sh_flags     = sh32->sh_flags;
      sh_table[i].sh_addr      = (Elf64_Addr) sh32->sh_addr;
      sh_table[i].sh_offset    = (Elf64_Off) sh32->sh_offset;
      sh_table[i].sh_size      = (uint64_t) sh32->sh_size;
      sh_table[i].sh_link      = sh32->sh_link;
      sh_table[i].sh_info      = sh32->sh_info;
      sh_table[i].sh_addralign = (uint64_t) sh32->sh_addralign;
      sh_table[i].sh_entsize   = (uint64_t) sh32->sh_entsize;
    }
    else
      read(fd, &sh_table[i], sizeof(Elf64_Shdr));
  }
}

char *epur_str(char *str)
{
  char *epured;
  int i;
  int j;

  j = 0;
  i = -1;
  epured = malloc(strlen(str) + 1);
  while (str[++i])
  {
    if (/*str[i] != '_' && str[i] != '@'*/ (str[i] >= 'a' && str[i] <= 'z')
    || (str[i] >= 'A' && str[i] <= 'Z')
    || (str[i] >= '0' && str[i] <= '9'))
    {
      (str[i] >= 'A' && str[i] <= 'Z' ? epured[j] = str[i] + 32
        : (epured[j] = str[i]));
      j++;
    }
  }
  epured[j] = '\0';
  return (epured);
}

int				compare_str(char *str1, char *str2)
{
  int		i;
  char  *str1_tmp;
  char  *str2_tmp;

  str1_tmp = epur_str(str1);
  str2_tmp = epur_str(str2);
  printf("%s %s \n",str1_tmp, str2_tmp);
  i = strcmp(str1_tmp, str2_tmp);
  if (strcmp(str1_tmp, str2_tmp) == 0)
  {
    //i = strncmp(str1, str2, (strlen(str1) > strlen(str2) ? strlen(str2) : strlen(str1)));
    /*(strlen(str1) > strlen(str2) ? str1[strlen(str2)] = '\0' :
     strlen(str2) > strlen(str1) ? str2[strlen(str1)] = '\0' : 0 );*/
    while (str1[0] == '_')
      str1++;
    while (str2[0] == '_')
      str2++;
    i = strcmp(str1, str2);
  }
  free(str1_tmp);
  free(str2_tmp);
  return (i);
}

void			add_to_list(t_list *list, Elf64_Sym *symb, void *str_ptr)
{
  t_list *tmp;
  Elf64_Sym *sym_tmp;

  tmp = list;
  if (list->sym_tbl == NULL)
    list->sym_tbl = symb;
  else if (compare_str(str_ptr + list->sym_tbl->st_name,
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
    while (list->next && compare_str(str_ptr + list->next->sym_tbl->st_name,
       str_ptr + symb->st_name) <= 0)
      list = list->next;
    tmp = list->next;
    list->next = malloc(sizeof(t_list));
    list->next->sym_tbl = symb;
    list->next->next = tmp;
  }
}

Elf64_Sym *read_section(Elf64_Ehdr *eh, int fd, Elf64_Shdr sh, void *str_ptr, t_list *list)
{
  Elf64_Sym *buff;
  Elf32_Sym *buff_32;
  int i;
  i = -1;

  lseek(fd, (off_t)sh.sh_offset, SEEK_SET);
  while (++i * (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
     sizeof(Elf32_Sym) : sizeof(Elf64_Sym)) < sh.sh_size)
  {
    buff = malloc(sizeof(Elf64_Sym));
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
    {
      buff_32 = malloc(sizeof(Elf32_Sym));
      read(fd, buff_32, sizeof(Elf32_Sym));
      buff->st_name = buff_32->st_name;
      buff->st_info = buff_32->st_info;
      buff->st_other = buff_32->st_other;
      buff->st_shndx = buff_32->st_shndx;
      buff->st_value = (Elf64_Addr) buff_32->st_value;
      buff->st_size = (uint64_t) buff_32->st_size;
    }
    else
      read(fd, buff, sizeof(Elf64_Sym));
    if (strlen((str_ptr + buff->st_name)) > 0 &&
    (buff->st_shndx == SHN_ABS ? (ELF64_ST_BIND(buff->st_info) == STB_LOCAL ? 0 : 1) : 1))
      add_to_list(list, buff, str_ptr);
    }
	 return (buff);
}

void print_symbol_table(int fd,	Elf64_Ehdr *eh, Elf64_Shdr *sh_table, int symbol_table)
{
	char *str_ptr;
	int str_tbl_ndx;
  t_list *list;

  list = malloc(sizeof(t_list)); list->next = NULL; list->sym_tbl = NULL;
  str_tbl_ndx = sh_table[symbol_table].sh_link;
  str_ptr = malloc(sh_table[str_tbl_ndx].sh_size);
  lseek(fd, (off_t)sh_table[str_tbl_ndx].sh_offset, SEEK_SET);
	read(fd, str_ptr, sh_table[str_tbl_ndx].sh_size);

  read_section(eh, fd, sh_table[symbol_table], str_ptr, list);
	while (list)
  {
    if (strlen(str_ptr + list->sym_tbl->st_name) > 0)
    {
	    if (list->sym_tbl->st_shndx != SHN_UNDEF)
      {
        (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
          printf("%08lx ", list->sym_tbl->st_value) :
          printf("%016lx ", list->sym_tbl->st_value));
        printf("%c ", print_type(list->sym_tbl, sh_table));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
      else
      {
        (eh->e_ident[EI_CLASS] == ELFCLASS32 ?
          printf("%10c ", print_type(list->sym_tbl, sh_table)) :
            printf("%18c ", print_type(list->sym_tbl, sh_table)));
        printf("%s\n", (char *) (str_ptr + list->sym_tbl->st_name));
      }
    }
    list = list->next;
	}
}

void print_symbols(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;

  i = -1;
	while (++i < eh->e_shnum)
  {
		if ((sh_table[i].sh_type == SHT_SYMTAB)/*
        || (sh_table[i].sh_type == SHT_DYNSYM)*/)
    {
      //printf("\n[Section %03d]", i);
      print_symbol_table(fd, eh, sh_table, i);
		}
	}
}

int main(int ac, char *av[])
{
	int fd;
	Elf64_Ehdr *eh;
	Elf64_Shdr *sh_tbl;
  Elf64_Ehdr *tmp;
  Elf32_Ehdr *eh32;

	if (ac != 2)
		return (0);
	fd = open(av[1], O_RDONLY | O_SYNC);
	if (fd < 0)
  {
		printf("Unable to open %s\n", av[1]);
		return (0);
	}
  eh = malloc(sizeof(Elf64_Ehdr));
  lseek(fd, (off_t)0, SEEK_SET);
  read(fd, (Elf64_Ehdr *)eh, sizeof(Elf64_Ehdr));
  if (eh->e_ident[EI_CLASS] == ELFCLASS32)
  {
    tmp = malloc(sizeof(Elf64_Ehdr));
    memcpy(tmp, eh, sizeof(Elf64_Ehdr));
    eh32 = (Elf32_Ehdr *) tmp;

    eh->e_type = eh32->e_type;
    eh->e_machine = eh32->e_machine;
    eh->e_version = eh32->e_version;
    eh->e_entry =  (Elf64_Addr) eh32->e_entry;
    eh->e_phoff = (Elf64_Off) eh32->e_phoff;
    eh->e_shoff = (Elf64_Off) eh32->e_shoff;
    eh->e_flags = eh32->e_flags;
    eh->e_ehsize = eh32->e_ehsize;
    eh->e_phentsize = eh32->e_phentsize;
    eh->e_phnum = eh32->e_phnum;
    eh->e_shentsize = eh32->e_shentsize;
    eh->e_shnum = eh32->e_shnum;
    eh->e_shstrndx = eh32->e_shstrndx;
    free(tmp);
  }
  sh_tbl = malloc(sizeof(Elf64_Shdr) * eh->e_shnum);
  read_section_header_table(fd, eh, sh_tbl);
  print_symbols(fd, eh, sh_tbl);
	return 0;
}
