/* make.y
 parseur de make : les données sont sauvegardées au fur et à mesure
 du parsing.
 */
%{
# include "make.h"

/* Au niveau global, initialisation de la première commande à effectuer, et des deux tables de hâchage. */

char * firstCommand = NULL;
struct linkedList* variablesHash[SIZE] = { NULL };
struct linkedList* cmdsHash[SIZE] = { NULL };

int yylex(void);
int yyerror(char *);

# define YYDEBUG 1
%}

%union {
  int i;                        /* utilisé pour le niveau de variable $ */
  char * c;                     /* n'importe quelle chaîne */
  struct tokenList * toklist;   /* tokenList construite */
};

%token <c> YSTRING

%token <c> VAR_NAME

%token <i> VAR_SIGN

%token LINESEPARATOR

%type <c> .canexpendvar

%type <toklist> .recstrings

%%
program : /* rien */          /* point d'entree : rien que des expr */
	| program LINESEPARATOR
        | program expr
        ;

                                /* toutes les expressions */
expr : .canexpendvar ':' .recstrings LINESEPARATOR .recstrings LINESEPARATOR
		{
			if(firstCommand == NULL) {
			firstCommand = $1;
			}
			struct value * cmdValue = createCmdValue($3, $5);
			insert($1, cmdValue, cmdsHash);
		}
        | YSTRING '=' .recstrings LINESEPARATOR
		{
			struct value * variableValue = createVariableValue($3);
			insert($1, variableValue, variablesHash);
		}
        ;

/*
  Ceci est problématique ici, il faudrait en effet pouvoir prendre une liste de chaînes, et construire plusieurs
  cmdValue struct à partir de chaque "target", vient aussi la question de la résolution de variables.
  Par mesure de simplicité, on fait juste valider une syntaxe simple ici.
*/
.canexpendvar : YSTRING
                { $$=$1; }
	| VAR_SIGN '(' YSTRING ')'
                { $$=$3; }
       	| VAR_NAME
                { $$=$1; }
       	;

.recstrings : /* rien */
		{ $$ = createTokenList(); }
          | YSTRING .recstrings
		{
			$$=$2;
			addToTokenList($1, $2, 0);
		}
          | VAR_SIGN '(' YSTRING ')' .recstrings
               {
			$$=$5;
			addToTokenList($3, $5, $1);
                }
          | VAR_NAME .recstrings
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

	/* Prend le makefile dans le dossier courant */
        char * makefileName = "Makefile";
        char * fullPath = (char *) calloc((strlen(makefileName)+strlen(cwd)+1)*sizeof(char), sizeof(char));

        sprintf(fullPath, "%s/%s", cwd, makefileName);

        yyin = fopen(fullPath, "r");
        yyparse();

        char bufferCmdLineVar[10] = {'\0'};
        struct value * variableValue;
        /* Passe en variable tous les arguments de la commande pour référence dans le makefile */
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
       enum Bool commandHasBeenCalled = callCommmand(firstCommand, cwd);

	/* Si aucun appel */
       if (!commandHasBeenCalled) {
           printf("Up to Date\n");
       }

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

Différence avec GNU make:
	l'indentation. Une règle unique.

Done:
Multiline
Make Makefile
Single Line variable
Make Makefile take command line argument & variables
Clean up memory
popen with printing
problème avec résolution des variables qui est une chaîne
Check that popen prints
Refactor - especially part of cmdsresolved and depresolved
Implicit rules for GCC
https://www.gnu.org/software/make/manual/html_node/Catalogue-of-Rules.html#Catalogue-of-Rules - just one
Empty Variables resolution



TODO 1 - Parse multiple targets - not in the data structure but in the syntaxical analysis
TODO 2 - Multiple commands per target - Change indent
TODO 3 - make it recognize different assignment characters such as ":=", etc...
TODO 4 - Cleanup Code and memory
TODO 5 - Clean up grammar
TODO 6 - Error Management
TODO 7 - Test files
TODO 8 - Bring in phony

*/