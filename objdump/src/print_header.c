/*
** print_header.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Sun Feb 26 21:23:10 2017 Full Name
** Last update	Sun Feb 26 21:42:33 2017 Full Name
*/

#include  "objdump.h"

int get_nb_flag(int flag)
{
	int nb;

	nb = 0;
	if (flag == (flag | EXEC_P))
		nb++;
	if (flag == (flag | DYNAMIC))
		nb++;
	if (flag == (flag | HAS_SYMS))
		nb++;
	if (flag == (flag | HAS_RELOC) && flag !=  (flag | DYNAMIC)
		&& flag != (flag | EXEC_P))
		nb++;
	if (flag == (flag | D_PAGED))
		nb++;
	return (nb);
}

void aff_flags(int flag)
{
	int nb;

	nb = get_nb_flag(flag) - 1;
	if (flag == (flag | EXEC_P))
		printf("EXEC_P%s", nb-- > 0 ? ", " : "");
	if (flag == (flag | HAS_RELOC) && flag !=  (flag | DYNAMIC)
		&& flag != (flag | EXEC_P))
		printf("HAS_RELOC%s", nb-- > 0 ? ", " : "");
	if (flag == (flag | HAS_SYMS))
		printf("HAS_SYMS%s", nb-- > 0 ? ", " : "");
	if (flag == (flag | DYNAMIC))
		printf("DYNAMIC%s", nb-- > 0 ? ", " : "");
	if (flag == (flag | D_PAGED))
		printf("D_PAGED%s", nb-- > 0 ? ", " : "");
	printf("\n");
}

void print_flags(Elf64_Ehdr *eh, Elf64_Shdr *sh_tbl)
{
	int flag;
	int i;

	i = -1;
	flag = BFD_NO_FLAGS;
	if (eh->e_type == ET_EXEC)
		flag |= EXEC_P;
	if (eh->e_type == ET_DYN)
		flag |= DYNAMIC;
	while (++i < eh->e_shnum)
	{
		if (sh_tbl[i].sh_type == SHT_SYMTAB)
			flag |= HAS_SYMS;
		if (sh_tbl[i].sh_type == SHT_RELA && eh->e_type != ET_DYN
			&& eh->e_type != ET_EXEC)
			flag |= HAS_RELOC;
		if (sh_tbl[i].sh_type == SHT_DYNAMIC)
			flag |= D_PAGED;
	}
	printf(", flags 0x%08x:\n", flag);
	aff_flags(flag);
}

int	print_header(Elf64_Ehdr *eh, Elf64_Shdr *sh_tbl, char *name)
{
  printf("\n%s:     file format %s\n", name, "elf64-x86-64");
  printf("architecture: %s",
   (eh->e_machine == EM_X86_64 ? "i386:x86-64" : "UNKNOWN!"));
 	print_flags(eh, sh_tbl);
	printf("start address 0x%0*lx\n\n", eh->e_machine == EM_X86_64 ? 16 : 8,
   eh->e_entry);
  return (1);
}
