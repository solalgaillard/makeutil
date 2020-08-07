/* make.y
 parseur de make : les données sont sauvegardées au fur et à mesure
 du parsing.
 */
%{
# include "make.h"
char firstCommand[1000] = {'\0'};
struct linkedList* variablesHash[SIZE] = {NULL};
struct linkedList* cmdsHash[SIZE] = {NULL};

int yylex(void);
int yyerror(char *);

# define YYDEBUG 1
%}

%union {
  int i;                        /* constantes, etiquettes et nbre d'arg. */
  char * c;                     /* variables et fonctions */
  struct tokenList * toklist;             /* expressions */
};

%token <c> YNAME

%token <i> DOLL_SIGN

%token NEWLINE

%type <c> expr

%type <toklist> .listnames

%%
programme : /* rien */          /* point d'entree : rien que des fonctions */
        | programme NEWLINE
        | programme expr
        ;

                                /* toutes les expressions */
expr    : YNAME ':' .listnames NEWLINE .listnames NEWLINE
                {
                    if(firstCommand[0] == '\0') {
                        strcpy(firstCommand,$1);
                    }
                    struct value * cmdValue = createCmdValue($3, $5);
                    insert($1, cmdValue, cmdsHash);
                    printf("NO bug")
                 }
        | YNAME '=' .listnames NEWLINE
            {
            struct value * variableValue = createVariableValue($3);
            insert($1, variableValue, variablesHash);
            }
        ;

.listnames
        :
            { $$ = createTokenList();}
          | YNAME .listnames
             { $$=$2;
             printf("\n\n%s\n\n", $1);
             addToTokenList($1, $2, 0);
              }
          | DOLL_SIGN '(' YNAME ')' .listnames
               {
                $$=$5;
                addToTokenList($3, $5, $1);
                }


%%

# include "lex.yy.c"            /* yylex et sa clique */
extern FILE *yyin;

int
main(int argc, char *argv[]){
    char* cwd;
    if( (cwd=getcwd(NULL, 0)) == NULL) {
        perror("failed to get current directory\n");
    }
    else {
        printf("%s \nLength: %zu\n", cwd, strlen(cwd));
        char fullpath[2000] = {'\0'};
        sprintf(fullpath, "%s/Makefile", cwd);

        yyin = fopen(fullpath, "r");
        yyparse();

       callCommmand(firstCommand, cwd);
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


/*
TODO - Multiline - d
TODO - Make Makefile - d
TODO 1 - Make Makefile take command line argument & variables
TODO 2 - Refactor
TODO 3 - Error Management
TODO 4 - Clean up memory
TODO 5 - Test files
TODO 6 - Bing in phony
TODO 7 - Investigate new features
*/