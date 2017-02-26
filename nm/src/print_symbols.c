/*
** print_symbols.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Sun Feb 26 21:27:45 2017 Full Name
** Last update	Sun Feb 26 21:28:44 2017 Full Name
*/

#include	"nm.h"

void		read_section(t_core *core, Elf64_Shdr *sh, void *str_ptr, t_list *list)
{
  Elf64_Sym	*buff;
  int		i;

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

void		print_symbol_table(t_core *core, char *str_ptr,
				   t_list *list, char *sh_strtab_p)
{
  while (list)
  {
    if (strlen(str_ptr + list->sym_tbl->st_name) > 0)
    {
	    if (list->sym_tbl->st_shndx != SHN_UNDEF)
      {
        printf("%0*lx ", (IS_32 ? 8 : 16),
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

void		init_symbol_table(int fd, t_core *core, int symbol_table)
{
  char		*str_ptr;
  char		*sh_strtab_p;
  int		str_tbl_ndx;
  t_list	*list;
  Elf64_Shdr	*sh_strtab;

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

void		print_symbols(int fd, t_core *core)
{
  int		i;

  i = -1;
  while (++i < core->eh->e_shnum)
  {
    if (core->sh_tbl[i].sh_type == SHT_SYMTAB)
      init_symbol_table(fd, core, i);
  }
}
