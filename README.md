## Introduction

I decided to create a make utility. This utility behaves almost like GNU's but remains limited by some aspects.

### It can :
- Define variables. `CC = gcc`

- Define target files/constructions.
`
make: $O
`
- Define necessary sources and observe any change in order to rebuild targets.
`
make.tab.o : make.tab.c lex.yy.c
`
- Replace recursively variables by their values.
```
FLAG_EX = -O
CC = gcc $(FLAG_EX)
OTHER = $(CC) ----> gcc -O
```
- Place commande line arguments in variables that are accessible in the *Makefile*. `echo $0`
- Use implicit rules for the construction object files (works for any dependency that is without a target).
```
make.tab.o : make.tab.c lex.yy.c ----> gcc -O -DECHO -c -o expr.o expr.c
```
- Accept comments.
```
#
# Makefile pour make
```
- Permettre le retour à la ligne avec un caractère spécial sans empêcher le « parsing » de l’expression.
```
CFLAGS = -O \
-DECHO
```

### It cannot :
- Assign more that one command per target.
```
echo:
    echo "commande 1"
    echo "commande 2"
```
- Create many targets from a variable.
```
O = make.tab.o expr.o hash_table.o # objets
$O : make.h
````
- Parse many targets for the same sources and the same command. `make.tab.o expr.o hash_table.o : make.h`

### It is particularly different from the GNU utility on these aspects :
- You don't need to tabulate in order to separate the sources from the command, you will need a newline (I would have to revise the parser to allow this and if I did, we could then have multiple commands for one target, with the tab becoming the delimiter and not anymore the newline).
- It does not recognize all assignment tokens such as
« := » but only « = »
- Does not allow the use of .PHONY.

## To build
Nothing easier, obviously, use make `$ make`

Then, if you to play with it, duplicate the folder, clear all object files, use the make program thus created instead of the official GNU make to be rebuild our make.

## Remarks:
Obviously, the original utility is much more complete. However, my utility allows for fairly complex Makefiles and covers most of the basic uses of *Make*. The biggest limitation being that you can only call one command for each target. By revising a little the data structure which contains all the variables and command resolutions as well as the parser, we could add this functionality. Moreover, to test my utility in real conditions, I have been using the program's own Makefile. It is able to perform all commands, except for the extension of targets in `$ O: make.h`
