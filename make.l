/* make.l
     analyseur lexical. representation unique des chaines de caracteres
*/
    int lineno = 1;
%x BACKSLASH
%%

<INITIAL,BACKSLASH>
    \\          { BEGIN(BACKSLASH); }
    \r?\n                   { printf("SHould not bve active");return NEWLINE;}

<BACKSLASH>\r?\n    {    printf("IT SHOULD HAVE ENDED"); BEGIN(INITIAL);          }



[a-zA-Z_.-][a-zA-Z0-9_.-]*  { yylval.c = strdup(yytext); return YNAME; }
$+                      { yylval.i = strlen(yytext); return DOLL_SIGN;}
[=():]                  { return yytext[0]; }
<<EOF>>                 { static int once = 0; return (once = !once) ? NEWLINE : 0; }
#.*                     { ; /* Commentaire */ }
%%

yywrap(){ return 1; }
