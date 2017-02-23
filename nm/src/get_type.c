/*
** get_type.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Tue Feb 21 13:40:24 2017 Full Name
** Last update	Thu Feb 23 16:53:50 2017 Full Name
*/

#include      "nm.h"

static char	by_shdr_name(const char *name)
{
  if (strcmp(name, ".bss") == 0)
    return ('b');
  if (strcmp(name, ".code") == 0 || strcmp(name, ".fini") == 0 ||
      strcmp(name, ".init") == 0 || strcmp(name, ".text") == 0 ||
      strcmp(name, ".fini_array") == 0 || strcmp(name, ".init_array") == 0)
    return ('t');
  if (strcmp(name, ".data") == 0)
    return ('d');
  if (strcmp(name, ".debug") == 0)
    return ('N');
  if (strcmp(name, ".drectve") == 0 || strcmp(name, ".idata") == 0)
    return ('i');
  if (strcmp(name, ".edata") == 0)
    return ('e');
  if (strcmp(name, ".pdata") == 0)
    return ('p');
  if (strcmp(name, ".rdata") == 0 || strcmp(name, ".rodata") == 0)
    return ('r');
  if (strcmp(name, ".sbss") == 0)
    return ('s');
  if (strcmp(name, ".scommon") == 0)
    return ('c');
  if (strcmp(name, ".sdata") == 0)
    return ('g');
  return ('?');
}


char            print_type(Elf64_Sym *sym, Elf64_Shdr *shdr, char *sh_strtab_p)
{
  char  c;

  c = '?';
  if (ELF64_ST_BIND(sym->st_info) == STB_GNU_UNIQUE)
    c = 'u';
  else if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
  {
    c = (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT) ? 'V' : 'W';
    if (sym->st_shndx == SHN_UNDEF)
      return (c += 32);
  }
  else if (sym->st_shndx == SHN_UNDEF)
    c = 'U';
  else if (sym->st_shndx == SHN_COMMON)
    c = 'C';
  else if (sym->st_shndx == SHN_ABS)
    c = 'A';
  else if (sym->st_shndx != SHN_ABS)
      {
        c = by_shdr_name(sh_strtab_p +  shdr[sym->st_shndx].sh_name);
        if (c != '?')
          {
            if (ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
          c -= 32;
          return(c);
        }
      }
  if (c != '?')
  {
     if (ELF32_ST_BIND(sym->st_info) == STB_LOCAL)
       c += 32;
      return (c);
    }
/*  if (sym->st_shndx == SHT_SHLIB)
    c = 'N'; */
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
/*  else if (shdr[sym->st_shndx].sh_type ==  SHT_INIT_ARRAY)
    return ('t'); */
/*  else if ((shdr[sym->st_shndx].sh_flags & SHF_WRITE) != SHF_WRITE
    && (shdr[sym->st_shndx].sh_flags & SHF_EXECINSTR) != SHF_EXECINSTR)
    {
      if (shdr->sh_type == SHT_GROUP)
 	      return ('n');
      return ('r');
    }  */
  else if ((shdr->sh_flags & SHF_WRITE) != SHF_WRITE)
    {
      if (shdr->sh_type == SHT_GROUP)
	      return ('n');
      return ('r');
    }
  else if (shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_DYNAMIC)
    return ('d');
  if (c != '?' && ELF32_ST_BIND(sym->st_info) == STB_LOCAL)
    c += 32;
  return c;
}
