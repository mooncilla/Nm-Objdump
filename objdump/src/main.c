  /*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Thu Feb 16 11:09:24 2017 Full Name
** Last update	Sat Feb 18 00:33:15 2017 Full Name
*/

#include      "objdump.h"

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

int	print_header(Elf64_Ehdr *eh, char *name)
{
  printf("\n%s:\tfile format %s\n", name, "elf64-x86-64");
  printf("architecture: %s, flags 0x%08x:\n",
   (eh->e_machine == EM_X86_64 ? "i386:x86-64" : "UNKNOWN!"),
	 eh->e_flags);
  printf("start address 0x%0*lx\n\n", eh->e_machine == EM_X86_64 ? 16 : 8,
   eh->e_entry);
  return (1);
}

unsigned int  print_s_line_bytes(char *s_str, unsigned int	shdr_end,
  Elf64_Shdr *shdr, char	tmp[17])
{
  unsigned int	i;
  unsigned int	j;

  i = 0;
  while (i < 16 && shdr_end + i < shdr->sh_size)
  {
    j = 0;
    printf(" ");
    while (j < 4 && shdr_end + i + j < shdr->sh_size)
     {
       printf("%02x", (unsigned char) s_str[shdr_end + i + j]);
        tmp[i + j] = (isprint(s_str[shdr_end + i + j])
         ? s_str[shdr_end + i + j] : '.');
        ++j;
     }
     i += j;
  }
    return (i);
}

void	print_section(char *s_str, Elf64_Shdr *shdr)
{
  char		tmp[17];
  unsigned int	shdr_end;
  unsigned int	i;

  shdr_end = 0;
  while (shdr_end < shdr->sh_size)
  {
    memset(tmp, ' ', 16);
    printf(" %04lx", shdr->sh_addr + shdr_end);
    i = print_s_line_bytes(s_str, shdr_end, shdr, tmp);
    shdr_end += i;
    while (i < 16)
    {
      if (i % 4 == 0)
        printf(" ");
      printf("  ");
      i++;
    }
    tmp[16] = '\0';
    printf("  %s\n", tmp);
  }
}

void print_section_tables(int fd, Elf64_Ehdr *eh, Elf64_Shdr *sh_table)
{
	int i;
  char *str_ptr;
  Elf64_Shdr *sh_strtab;
  char *sh_strtab_p;

  i = -1;
  sh_strtab = &sh_table[eh->e_shstrndx];
  sh_strtab_p = malloc(sh_strtab->sh_size);
  lseek(fd, (off_t)sh_strtab->sh_offset, SEEK_SET);
  read(fd, sh_strtab_p, sh_strtab->sh_size);
	while (++i < eh->e_shnum)
  {
    if (sh_table[i].sh_type != SHT_NOBITS
      && sh_table[i].sh_type != SHT_SYMTAB
      && sh_table[i].sh_type != SHT_NULL
      && (sh_table[i].sh_type != SHT_STRTAB
      || sh_table[i].sh_flags & SHF_ALLOC))
    {
      str_ptr = malloc(sh_table[i].sh_size);
      lseek(fd, (off_t)sh_table[i].sh_offset, SEEK_SET);
      read(fd, str_ptr, sh_table[i].sh_size);
      printf("Contents of section %s:\n",
      sh_strtab_p +  sh_table[i].sh_name);
      print_section(str_ptr, &sh_table[i]);
      free(str_ptr);
    }
	}
  free(sh_strtab_p);
}

void launch_objdump(int fd, int ac, char *name, char *file_name)
{
  Elf64_Ehdr *eh;
  Elf64_Shdr *sh_tbl;

  eh = malloc(sizeof(Elf64_Ehdr));
  lseek(fd, (off_t)0, SEEK_SET);
  read(fd, (Elf64_Ehdr *)eh, sizeof(Elf64_Ehdr));
  if (is_ELF(eh, name, (ac == 1) ? "a.out" : file_name) == 1)
  {
    if (eh->e_ident[EI_CLASS] == ELFCLASS32)
      fill_Eh32(eh, *eh);
    if (lseek(fd, eh->e_shoff, SEEK_SET)
     + (eh->e_shentsize * eh->e_shnum) > lseek(fd, 0, SEEK_END))
      fprintf(stderr, "%s: %s: File truncated\n", name, file_name);
    else
    {
      sh_tbl = malloc(sizeof(Elf64_Shdr) * eh->e_shnum);
      read_section_header_table(fd, eh, sh_tbl);
      print_header(eh, (ac == 1) ? "a.out" : file_name);
      print_section_tables(fd, eh, sh_tbl);
      free(eh);
      free(sh_tbl);
    }
  }
}

int main(int ac, char *av[])
{
	int fd;
  int i;

  i = (ac == 1 ? -1 : 0);
  while (++i < ac)
  {
    fd = open((ac == 1) ? "a.out" : av[i], O_RDONLY | O_SYNC);
	  if (fd > 0)
    {
      launch_objdump(fd, ac, av[0], (ac == 1) ? "a.out" : av[i]);
    }
    else
      printf("%s: '%s': No such file\n", av[0], av[i]);
  }
	return 0;
}
