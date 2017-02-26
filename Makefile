##
## Makefile for Makefile in /home/gastal_r/rendu/PSU_2016_malloc
##
## Made by
## Login   <gastal_r>
##
## Started on  Fri Jan 27 12:28:44 2017
## Last update Sat Feb 11 14:27:40 2017 
##

CC		=       gcc

RM		=       rm -rf

CFLAGS		+=	-Wall -Wextra -W -Werror
CFLAGS		+=	-I./nm/includes/
CFLAGS		+=	-I./objdump/includes/

NMNAME		=	my_nm

NMSRCS		=	nm/src/main.c			\
			nm/src/fill_32_struct.c		\
			nm/src/get_type.c		\
			nm/src/print_symbols.c

NMOBJS		=	$(NMSRCS:.c=.o)

ODNAME		=	my_objdump

ODSRCS		=	objdump/src/main.c		\
			objdump/src/fill_32_struct.c	\
			objdump/src/print_section.c	\
			objdump/src/print_header.c

ODOBJS		=	$(ODSRCS:.c=.o)

all		:	$(NMNAME) $(ODNAME)

nm		:	$(NMNAME)

objdump		:	$(ODNAME)

$(NMNAME)	:	$(NMOBJS)
			$(CC) -o $(NMNAME) $(NMOBJS) $(CFLAGS) $(NMINCLUDE)

$(ODNAME)	:	$(ODOBJS)
			$(CC) -o $(ODNAME) $(ODOBJS) $(CFLAGS) $(ODINCLUDE)

clean		:
			$(RM) $(NMOBJS)
			$(RM) $(ODOBJS)

fclean		:       clean
			$(RM) $(NMNAME)
			$(RM) $(ODNAME)

re		:       fclean all

.PHONNY		:       all clean fclean re
