/*
** print_section.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Sun Feb 26 02:42:23 2017 Full Name
** Last update	Sun Feb 26 22:14:43 2017 Full Name
*/

#include	"objdump.h"

unsigned int	hexa_length(unsigned x)
{
   int		n;
   
   if (x <= 16)
     return (1);
   n = 0;
   while (x != 0)
   {
     x /= 16;
     ++n;
   }
   return (n);
}

unsigned int	print_s_line_bytes(char *s_str, unsigned int shdr_end,
				   Elf64_Shdr *shdr, char tmp[17])
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

void		print_section(char *s_str, Elf64_Shdr *shdr)
{
  char		tmp[17];
  unsigned int	shdr_end;
  unsigned int	i;
  unsigned int	hex_l;

  shdr_end = 0;
  while (shdr_end < shdr->sh_size)
  {
    memset(tmp, ' ', 16);
    hex_l = hexa_length(shdr->sh_addr +
     (shdr->sh_size - ((shdr->sh_size%16 == 0 ? 16 : shdr->sh_size%16))));
    printf(" %0*lx", (hex_l <= 4 ? 4 : hex_l),
    shdr->sh_addr + shdr_end);
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

void		print_section_tables(int fd, Elf64_Ehdr *eh,
				     Elf64_Shdr *sh_tbl, Elf64_Shdr *sh_strtab)
{
  int		i;
  char		*str_ptr;
  char		*sh_strtab_p;

  i = -1;
  sh_strtab_p = malloc(sh_strtab->sh_size);
  lseek(fd, (off_t)sh_strtab->sh_offset, SEEK_SET);
  read(fd, sh_strtab_p, sh_strtab->sh_size);
  while (++i < eh->e_shnum)
  {
    if (sh_tbl[i].sh_type != SHT_NOBITS && sh_tbl[i].sh_type != SHT_SYMTAB
      && sh_tbl[i].sh_type != SHT_NULL && (sh_tbl[i].sh_type != SHT_STRTAB
      || sh_tbl[i].sh_flags & SHF_ALLOC) && sh_tbl[i].sh_name)
    {
      str_ptr = malloc(sh_tbl[i].sh_size);
      lseek(fd, (off_t)sh_tbl[i].sh_offset, SEEK_SET);
      read(fd, str_ptr, sh_tbl[i].sh_size);
      printf("Contents of section %s:\n",
      sh_strtab_p +  sh_tbl[i].sh_name);
      print_section(str_ptr, &sh_tbl[i]);
      free(str_ptr);
    }
  }
  free(sh_strtab_p);
}
