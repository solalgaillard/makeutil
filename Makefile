#
# Makefile pour le Plus Petit Compilateur qui Marche
#
# Pour compiler :
#		make a.out
# Si ca ne marche pas, remplacer gcc par cc, bison par yacc, et flex par lex.
#
CFLAGS = -O
CC = gcc

O = make.tab.o expr.o	hash_table.o			# objets
S = README Makefile expr.c hash_table.c make.h make.l make.y		# sources

clean:
	rm -f *.tab.c lex.yy.c make a.out core *.o *.output *~

a.out: $O
	$(CC) $(CFLAGS) $O

make.tab.c : make.y
	bison make.y

lex.yy.c : make.l
	flex make.l

make.tab.o : make.tab.c lex.yy.c

$O : make.h