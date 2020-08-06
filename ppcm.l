  /* ppcm.l
   analyseur lexical. representation unique des chaines de caracteres
   */
  char * chaine();
  int lineno = 1;

%%
[a-zA-Z_.-][a-zA-Z0-9_.-]*  { yylval.c = strdup(yytext); return YNAME; }
$+                      { yylval.i = strlen(yytext); return YVAR;}
[-+*/%=();:{},]          { return yytext[0]; }
[ \t\f]                 ;
\r?\n                   {return NEWLINE;}
<<EOF>>                 { static int once = 0; return (once = !once) ? NEWLINE : 0; }
.                       { fprintf(stderr, "yylex : (%c)\n", yytext[0]); }
"/*"([^*]|("*"+[^/*]))*"*"+"/"  { ; /* Commentaire */ }
%%
static char * chaines[MAXNOM];
static int nchaines = 0;

/* chaine  --  renvoie une representation unique de la chaine argument */
char *
chaine(char * s){
  /*register char **p, **f;

  for(p = & chaines[0], f = p + nchaines; p < f; p += 1)
    if (strcmp(*p, s) == 0)
      return *p;
  if (nchaines == MAXNOM){
    fprintf(stderr, "Pas plus de %d noms\n", MAXNOM);
    exit(1);
  }
  if ((*p = (char *)malloc(strlen(s) + 1)) == NULL)
    nomem();
  strcpy(*p, s);
  nchaines += 1;
  return *p;*/

  printf("print back %s", s);


}

yywrap(){ return 1; }
