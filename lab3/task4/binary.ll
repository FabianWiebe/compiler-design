%top{
	#include <string>
	#include "binary.tab.hh"
	#include "Node.hh"
	#define YY_DECL yy::parser::symbol_type yylex()
}
%x STRING
%option noyywrap nounput batch noinput
NOT_SPECIAL [^\n|; $\t\\'"=<()]
%%
({NOT_SPECIAL}|\\.)*					{ return yy::parser::make_WORD(Node("WORD", yytext)); }
\n           							{ return yy::parser::make_NEWL(yytext); }
\|           							{ return yy::parser::make_PIPE(yytext); }
=           							{ return yy::parser::make_EQUALS(Node("equals", yytext)); }
\<\(           							{ return yy::parser::make_SHELL_BEGIN(yytext); }
\)           							{ return yy::parser::make_SHELL_END(yytext); }
;           							{ return yy::parser::make_SEMI(yytext); }
[ \t]+									{ return yy::parser::make_BLANK(yytext); }
<INITIAL,STRING>${NOT_SPECIAL}+			{ std::string text(yytext);
											return yy::parser::make_VAR(Node("VAREXP", text.substr(1))); }
'([^'\\]|\\.)*'							{ std::string text(yytext);
					return yy::parser::make_QUOTE(Node("QUOTED", text.substr(1, text.length()-2))); }
\"										{ BEGIN(STRING); }
<STRING>\"								{ BEGIN(INITIAL); }
<STRING>([^"$\\]|\\.)+					{ return yy::parser::make_DBLQUOTE(Node("DBLQUOTE", yytext)); }
\"\"									{ return yy::parser::make_DBLQUOTE(Node("DBLQUOTE", "")); }
<<EOF>>    								   return yy::parser::make_END();
%%
