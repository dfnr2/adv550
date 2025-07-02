#
# Makefile for UNIX keyed-IO routines.
# 	(c) Ken Wellsch 1985
#
CC = /opt/local/bin/gcc
CFLAGS = -O
KLIB = klib.a

SRC =	kioglob.c GetBlk.c GetRec.c MakBlk.c MakNam.c \
	MapBlk.c MapKey.c PutBlk.c PutRec.c \
	closek.c creatk.c dupk.c openk.c readk.c writek.c util.c 

OBJ =	GetBlk.o GetRec.o MakBlk.o MakNam.o \
	MapBlk.o MapKey.o PutBlk.o PutRec.o \
	closek.o creatk.o kioglob.o openk.o \
	dupk.o readk.o writek.o util.o 

HEADER = kio.h

all: $(KLIB)

.c.o: $(HEADER)
	$(CC) $(CFLAGS) -c $<

$(KLIB): $(OBJ)
	ar ru $(KLIB) $(OBJ)
	ranlib $(KLIB)

test: test-path

test-path: test_path
	./test_path

test_path: test_path.c $(KLIB)
	$(CC) $(CFLAGS) test_path.c $(KLIB) -o test_path

clean:
	rm -f *.o test_path

backup:
	cp Makefile $(HEADER) $(SRC) bkup

print:
	lpr -Phw -p -J "Keyed I/O Lib" Makefile $(HEADER) $(SRC)

lint:
	lint $(SRC)
