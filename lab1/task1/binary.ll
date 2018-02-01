%top{
#include "binary.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}
%option noyywrap nounput batch noinput
%%
[01]         { return yy::parser::make_BIT(yytext); }
[ \t]+	    { return yy::parser::make_NEW_NUMBER(yytext); }
\n           /* munch */
<<EOF>>       return yy::parser::make_END();
%%
