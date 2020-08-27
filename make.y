/* make.y
 parseur de make : les données sont sauvegardées au fur et à mesure
 du parsing.
 */
%{
# include "make.h"

char * firstCommand = NULL;
struct linkedList* variablesHash[SIZE] = { NULL };
struct linkedList* cmdsHash[SIZE] = { NULL };

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

%token <c> YSINGLELETTERNAME

%token <i> DOLL_SIGN

%token NEWLINE

%type <c> canexpendvar

%type <c> expr

%type <toklist> .listnames

%%
programme : /* rien */          /* point d'entree : rien que des fonctions */
        | programme NEWLINE
        | programme expr
        ;

                                /* toutes les expressions */
expr    : canexpendvar ':' .listnames NEWLINE .listnames NEWLINE
                {
                    if(firstCommand == NULL) {
                        firstCommand = $1;
                    }
                    struct value * cmdValue = createCmdValue($3, $5);
                    printf("CMD TARGET %s\n",$1);
                    insert($1, cmdValue, cmdsHash);
                 }
        | YNAME '=' .listnames NEWLINE
            {
            printf("VAR TARGET %s\n",$1);
            struct value * variableValue = createVariableValue($3);
            insert($1, variableValue, variablesHash);
            }
        ;

canexpendvar : YNAME
                {$$=$1;}
               | DOLL_SIGN '(' YNAME ')'
                {$$=$3;}
               | YSINGLELETTERNAME
                {$$=$1;}
        ;

.listnames
        :
            { $$ = createTokenList();}
          | YNAME .listnames
             { $$=$2;
             addToTokenList($1, $2, 0);
              }
          | DOLL_SIGN '(' YNAME ')' .listnames
               {
                $$=$5;
                addToTokenList($3, $5, $1);
                }
           | YSINGLELETTERNAME .listnames
                  {
                   $$=$2;
                   addToTokenList($1, $2, 1);
                   }


%%

# include "lex.yy.c"            /* yylex et sa clique */
extern FILE *yyin;

int
main(int argc, char *argv[]){

    char* cwd;

    if( (cwd=getcwd(NULL, 0)) == NULL) {
        perror("Problème pour l'obtention du répertoire courant.\n");
    }

    else {

        char * makefileName = "Makefile";

        char * fullPath = (char *) calloc((strlen(makefileName)+strlen(cwd)+1)*sizeof(char), sizeof(char));

        sprintf(fullPath, "%s/%s", cwd, makefileName);

        yyin = fopen(fullPath, "r");
        yyparse();

        char bufferCmdLineVar[10] = {'\0'};
        struct value * variableValue;
        for(int i = 0; i<argc; i++) {
            if(i==1) {
               firstCommand = argv[i];
            }

            struct tokenList * tokList = createTokenList();
            addToTokenList(strdup(argv[i]), tokList, 1);

            variableValue = createVariableValue(tokList);

            sprintf(bufferCmdLineVar, "%d",i);
            insert(strdup(bufferCmdLineVar), variableValue, variablesHash);

        }
       callCommmand(firstCommand, cwd);

       cleanUpMemory();

    }
  return 0;
}

int yyerror(char * message){
  extern int lineno;
  extern char * yytext;
  fprintf(stderr, "%d: %s at %s\n", lineno, message, yytext);
  return lineno;
}

/*
TODO - Multiline - d
TODO - Make Makefile - d
TODO - Single Line variable - d
TODO - Make Makefile take command line argument & variables - d
TODO - Clean up memory - d
TODO - popen with printing - d
TODO - problème avec résolution des variables qui est une chaîne... next - d
TODO - Check that popen prints - d
TODO - Refactor - do that next - especially part of cmdsresolved and depresolved - d
TODO - Implicit rules for GCC - d
https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules - just one
TODO - Empty Variables ---> d
TODO - FIGURE OUT FLOW - WHY IT - d

DIFFERENCE AVEC MAKE, l'indentation. Une règle unique.



TODO - Parse multiple targets - not in the data structure but in the syntaxical analysis
TODO - Multiple commands per target - Change indent

TODO - Cleanup Code and memory



TODO - Clean up grammar
TODO 3 - Error Management
TODO 5 - Test files


TODO 6 - Bring in phony








*/