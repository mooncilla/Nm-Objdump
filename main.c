/*
** main.c for PSU_2016_nmobjdump
**
** Made by	Full Name
** Login	gastal_r
**
** Started on	Tue Feb 14 09:48:24 2017 Full Name
** Last update	Thu Feb 16 11:09:17 2017 Full Name
*/

#include  <stdlib.h>
#include  <unistd.h>
#include  <dlfcn.h>
#include  <stdio.h>
#include  <fcntl.h>
#include  <sys/mman.h>
#include  <sys/stat.h>
#include <elf.h>

void	toto()
{
	write(1, "test nm\n", 9);
}


int     main(int ac, char **av)
{
  int fd;
  void *buf;
  struct stat s;
  Elf64_Ehdr *elf;

  fd = open(av[1], O_RDONLY);
  if (fd != -1)
  {
    fstat(fd, &s);
    buf = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf != -1)
    {
      printf("mmap(%s) : %08x\n", av[1], buf);
      elf = buf;
      printf("Section Header Table : addr = %08x, nb = %d\n",
      elf->e_shoff, elf->e_shnum);
    }
    else
    {
      perror("mmap");
    }
  close(fd);
  }
}

/*
int main(int ac, char const *av[])
{
  if (ac <= 1)
    return (0);
  void *fd = dlopen(av[1], RTLD_LAZY);
  void (*ptr)() = dlsym(fd, "about");
  (*ptr)();
  dlclose(fd);
  return 0;
}
*/
