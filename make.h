/* ppcm.h
 constantes arbitraires, variables globales, prototypes de fonctions
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#define SIZE 4096

extern char * firstCommand;
extern struct linkedList* variablesHash[SIZE];
extern struct linkedList* cmdsHash[SIZE];

struct token {
    char * value;
    int variableLevel;
};

struct tokenList {
    int count;
    struct token ** tokens;
};

struct cmd {
    struct tokenList * dependencies; // ->Résoudre en checkant si commande
    struct tokenList * callableCmds;
};

struct value {
    enum {variable, cmd} variableOrCmdSwitch;
    union
    {
        struct tokenList * tokensInVariable;
        struct cmd * cmd ;
    };
};

struct linkedList {
    char * key;
    struct value * value;
    struct linkedList * next;
};

/*
    Déclaration des fonctions utilisées pour l'accès et l'insertion
    dans la table de hâchage
*/
void insert(char *, struct value *, struct linkedList * []);
struct value * search(char *, struct linkedList * []);

/*
    Déclaration des fonctions utilisées créer les structures de données
    et appeler la commande extraite de la structure
*/
struct tokenList * createTokenList(void);
struct cmd * createCmd(void);
struct value * createCmdValue(struct tokenList *, struct tokenList *);
struct value * createVariableValue(struct tokenList *);
void addToTokenList(char *, struct tokenList *, int);
void callCommmand(char *, char *);
