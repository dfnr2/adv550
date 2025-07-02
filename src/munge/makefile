#
# Makefile for the Adventure Compiler.
# 	(c) Ken Wellsch 1985
#
CC = /opt/local/bin/gcc
CFLAGS = -O
KLIB = ../kio/klib.a
MLIB = mlib.a

MAIN = munge.c

SRC =	buffers.c compile.c fio.c glob.c \
	majors.c parse.c symtab.c util.c

OBJ =	buffers.o compile.o fio.o glob.o \
	majors.o parse.o symtab.o util.o

HEADER = mdefs.h

munge: $(MAIN) $(MLIB)
	$(CC) $(CFLAGS) $(MAIN) $(MLIB) $(KLIB) -o munge
	size munge

.c.o: $(HEADER)
	$(CC) $(CFLAGS) -c $<

$(MLIB): $(OBJ)
	ar ru $(MLIB) $(OBJ)
	ranlib $(MLIB)

clean:
	rm -f *.o

backup:
	cp Makefile $(HEADER) $(MAIN) $(SRC) bkup

print:
	lpr -Phw -p -J "Munge" Makefile $(HEADER) $(MAIN) $(SRC)

lint:
	lint $(SRC) $(MAIN)
