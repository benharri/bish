%{
  #include <string.h>
  #include "y.tab.h"
%}

%%

\n { return NEWLINE;}

[ \t] {}

">" { return GT; }
"<" { return LT; }
">>" { return GTGT; }
"|" { return PIPE; }
"&" { return AMPERSAND; }
[^ \t\n][^ \t\n]* {
  yylval.string_val = strdup(yytext);
  return WORD;
}

%%