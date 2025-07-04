#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
#
CFLAGS = -O -Wno-deprecated-non-prototype
KLIB = ../kio/klib.a
ALIB = alib.a

MAIN =	adv.c

SRC =	error.c advglob.c cache.c command.c condition.c \
	exec.c process.c savgm.c say.c showop.c symtab.c \
	util.c values.c webster.c

OBJ =	error.o advglob.o cache.o command.o condition.o \
	exec.o process.o savgm.o say.o showop.o symtab.o \
	util.o values.o webster.o

HEADER = adefs.h

adv: $(ALIB)
	cc $(CFLAGS) $(MAIN) $(ALIB) $(KLIB) -o adv
	size adv

.c.o: $(HEADER)
	cc $(CFLAGS) -c $<

$(ALIB): $(OBJ)
	ar ru $(ALIB) $(OBJ)
	ranlib $(ALIB)

test:
	@echo "No tests defined for adv module yet"

clean:
	rm -f *.o

backup:
	cp Makefile $(HEADER) $(SRC) $(MAIN) bkup

lint:
	lint $(SRC) $(MAIN)

print:
	lpr -Phw -p -J "ADV" Makefile $(HEADER) $(MAIN) $(SRC)
