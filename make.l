/* make.l
     analyseur lexical. representation unique des chaines de caracteres
*/
    int lineno = 1;
%x BACKSLASH
%%

<INITIAL,BACKSLASH>
    \\          { BEGIN(BACKSLASH); }
    \r?\n       { lineno += 1; return NEWLINE;}

<BACKSLASH>\r?\n    { lineno += 1; BEGIN(INITIAL); }



$+[a-zA-Z0-9]                 {  yylval.c = strdup(yytext+(strlen(yytext)-1)); return YSINGLELETTERNAME; }
[a-zA-Z~\*_.-][a-zA-Z0-9~\*_.-]*  { yylval.c = strdup(yytext); return YNAME; }
$+                      { yylval.i = strlen(yytext); return DOLL_SIGN;}
[=():]                  { return yytext[0]; }
<<EOF>>                 { static int once = 0; return (once = !once) ? NEWLINE : 0; }
#.*                     { ; /* Commentaire */ }
%%

int yywrap(){ return 1; }
