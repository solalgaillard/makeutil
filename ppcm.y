/* ppcm.y
 parseur de ppcm : le code est genere au fur et a mesure du parsing
 */
%{
# include "ppcm.h"
int yylex(void);
int yyerror(char *);
int posexpr, incr;              /* deplacement ds la pile pour les variables */
char * fonction;                /* le nom de la fonction courante */

# define YYDEBUG 1
%}

%union {
  int i;                        /* constantes, etiquettes et nbre d'arg. */
  char * c;                     /* variables et fonctions */
  struct tokenList * strlist;             /* expressions */
};

/*

One structure dynamic array


substitution est faite par dictionnaire

2 dicts:
    1 - dependencies ?
    2 - var

Quelle est la structure de cible/dépendence?

dic key cible, liste dépendence

#TODO Dictionaire -> choose table - give index - for double
#TODO Make structure variable

var struct
    char * nom
    int isVarLevel entre 0 et lvl


Process:
1 - parse cmds, replace all tokens par expansion if in cmd dict
2 - parse variables within cmd, if isVarLevel active

*/

%token <c> YNAME

%token <i> YVAR

%token NEWLINE

%type <c> expr

%type <c> .variable
%type <strlist> .listnames

%left '='

%%
programme : /* rien */          /* point d'entree : rien que des fonctions */
        | programme expr
        ;

                                /* toutes les expressions */
expr    : NEWLINE
        | .variable ':' .listnames NEWLINE .listnames NEWLINE
                {
                    if(firstCommand[0] == '\0') {
                        strcpy (firstCommand,$1);
                    }
                    struct value * cmdValue = createCmdValue($3, $5);
                    insert($1, cmdValue, cmdsHash);
                    printf("NO bug")
                 }
        | .variable '=' .listnames NEWLINE
            {
            struct value * variableValue = createVariableValue($3);
            insert($1, variableValue, variablesHash);
            }
        ;


.variable
        : YNAME
            { $$=$1; }
        ;

.listnames
        :  /* rien */
            { $$ = createTokenList();}
          | YNAME .listnames
             { $$=$2;
             addToTokenList($1, $2, 0);
              }
          | YVAR '(' YNAME ')' .listnames
               { printf("TEST%d", $1); }


%%

# include "lex.yy.c"            /* yylex et sa clique */
extern FILE *yyin;
int
main(int argc, char *argv[]){

    memset(firstCommand, '\0', sizeof(char)*1000);
    char* cwd;
    if( (cwd=getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    }
    else {
        printf("%s \nLength: %zu\n", cwd, strlen(cwd));
        char * fullpath = (char*) malloc((strlen(cwd)+10)*sizeof(char));
        sprintf(fullpath, "%s/Makefile", cwd);
        free(cwd);
        printf("%s", fullpath);

        yyin = fopen(fullpath, "r");
        yyparse();
        free(fullpath);

        printf("\n%s\n", firstCommand);

        struct value * result = search(firstCommand, cmdsHash);

        struct tokenList * dependencies = result->cmd->dependencies;
        //struct tokenList * callableCmds = result->cmd->callableCmds;
        int i = 0;
        for(i=0; i < dependencies->count; i++) {
            token = tokens[i];
            char * token = dependencies->tokens[i]->value;
            int isVar = dependencies->tokens[i]->variableLevel;
            if(isVar){
                struct value * nestedResult = search(token, variablesHash);
                struct tokenList * tokensInVariable = result->tokensInVariable;
                //RECURSION ICI
                
            }
            printf("\n%s\n", token->value);
        }


        printf("find dependencies");



    }


  return 0;
}




yyerror(char * message){
  extern int lineno;
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", lineno, message, yytext);
}

nomem(){
  fprintf(stderr, "Pas assez de memoire\n");
  exit(1);
}

storevar(char * name) {
    printf("%s ", name);
}


/*
WHAT STRUCTURE ?

Store variable names, dictionary structure ?

Then linked list.

Store functions name. Expand as you go. Expand variables, expand regex


#Do postprocess

Keep list of commands expanded.



*/