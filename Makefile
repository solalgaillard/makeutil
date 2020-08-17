#
# Makefile pour make
#
# Pour compiler :
#		make
#
CFLAGS = -O
CC = gcc

O = make.tab.o expr.o hash_table.o # objets
S = README Makefile expr.c hash_table.c make.h make.l make.y		# sources

make: $O
	$(CC) $(CFLAGS) $O -o make

clean:
	rm -f *.tab.c lex.yy.c make a.out core *.o *.output *~

make.tab.c : make.y
	bison make.y

lex.yy.c : make.l
	flex make.l

make.tab.o : make.tab.c lex.yy.c

$O : make.h