/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Fri Feb 17 00:36:13 2017 Full Name
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


Elf64_Sym *read_section(int fd, Elf64_Shdr sh)
{
	//char* buff = malloc(sh.sh_size);
  Elf64_Sym *buff = malloc(sh.sh_size);

	lseek(fd, (off_t)sh.sh_offset, SEEK_SET);
	read(fd, buff, sh.sh_size);
	return (buff);
}

void			add_to_list(int fd, off_t offset, int nbSym, t_list *list)
{

}

void print_symbol_table(int fd,	Elf64_Ehdr eh, Elf64_Shdr *sh_table, int symbol_table)
{
	void *str_tbl;
	Elf64_Sym *sym_tbl;
	int i;
  int symbol_count;
  int str_tbl_ndx;
  t_list *list;

  list = malloc(sizeof(t_list));
  i = -1;
  str_tbl_ndx = sh_table[symbol_table].sh_link;
//  str_tbl = (void *) read_section(fd, sh_table[str_tbl_ndx]);
  str_tbl = malloc(sh_table[str_tbl_ndx].sh_size);
  lseek(fd, (off_t)sh_table[str_tbl_ndx].sh_offset, SEEK_SET);
	read(fd, str_tbl, sh_table[str_tbl_ndx].sh_size);

  symbol_count = (sh_table[symbol_table].sh_size / sizeof(Elf64_Sym));

  sym_tbl = read_section(fd, sh_table[symbol_table]);


//char            print_type(Elf64_Sym sym, Elf64_Shdr *shdr);
	while (++i < symbol_count)
  {
    if (strlen((str_tbl + sym_tbl[i].st_name)) > 0)
    {
	    if (sym_tbl[i].st_value > 0)
      {
        printf("%016x ", sym_tbl[i].st_value);
        printf("%c ", print_type(sym_tbl[i], sh_table));
        printf("%s\n", (str_tbl + sym_tbl[i].st_name));
      }
      else
      {
        printf("%18c ", print_type(sym_tbl[i], sh_table));
        printf("%s\n", (str_tbl + sym_tbl[i].st_name));
      }
    }
	}
}

void print_symbols(int fd, Elf64_Ehdr eh, Elf64_Shdr *sh_table)
{
	int i;

	for(i = 0; i < eh.e_shnum; i++)
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

	print_symbols(fd, *eh, sh_tbl);
	return 0;
}
