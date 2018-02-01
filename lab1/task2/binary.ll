%top{
#include "binary.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}
%x STRING
%option noyywrap nounput batch noinput
NOT_SPECIAL [^\n|; $\t\\'"]
%%
({NOT_SPECIAL}|\\.)*					{ return yy::parser::make_TEXT(yytext); }
\n           							{ return yy::parser::make_NL(yytext); }
\|           							{ return yy::parser::make_PIPE(yytext); }
;           							{ return yy::parser::make_SEMI(yytext); }
[ \t]+									{ return yy::parser::make_BLANK(yytext); }
<INITIAL,STRING>${NOT_SPECIAL}+			{ return yy::parser::make_VAR(yytext); }
'([^'\\]|\\.)*'							{ return yy::parser::make_QUOTE(yytext); }
\"										{ BEGIN(STRING); }
<STRING>\"								{ BEGIN(INITIAL); }
<STRING>([^"$\\]|\\.)+					{ return yy::parser::make_DBLQUOTE(yytext); }
\"\"									{ return yy::parser::make_EMPTYQUOTE(yytext); }
<<EOF>>    								   return yy::parser::make_END();
%%
