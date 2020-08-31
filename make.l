/* make.l
     analyseur lexical. representation unique des chaines de caracteres
*/
    int lineno = 1;
%x BACKSLASH
%%

<INITIAL,BACKSLASH> /* Pour échapper au LINESEPARATOR */
    \\          { BEGIN(BACKSLASH); }
    \r?\n       { lineno += 1; return LINESEPARATOR;}

<BACKSLASH>\r?\n    { lineno += 1; BEGIN(INITIAL); }



[^:#=$()\r\n\t\f\v ]+   { yylval.c = strdup(yytext); return YSTRING; } /* peut pas prendre \s, def ici: https://www.gnu.org/software/make/manual/make.html#Using-Variables  */
$+[a-zA-Z0-9]           { yylval.c = strdup(yytext+(strlen(yytext)-1)); return VAR_NAME; }
$+                      { yylval.i = strlen(yytext); return VAR_SIGN; }
[=():]                  { return yytext[0]; }
<<EOF>>                 { static int once = 0; return (once = !once) ? LINESEPARATOR : 0; }
#.*                     { ; /* Commentaire */ }
%%

int yywrap(){ return 1; }
