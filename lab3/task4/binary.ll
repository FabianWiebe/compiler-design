%top{
	#include <string>
	#include <memory>
	#include "binary.tab.hh"
	#include "Node.hh"
	#define YY_DECL yy::parser::symbol_type yylex()
}
%x STRING
%option noyywrap nounput batch noinput
NOT_SPECIAL [^\n|; $\t\\'"=<()+\-*/]
%%
({NOT_SPECIAL}|\\.)*					{ return yy::parser::make_WORD(
													std::make_unique<WordNode>("WORD", yytext)); }
\n           							{ return yy::parser::make_NEWL(yytext); }
\|           							{ return yy::parser::make_PIPE(yytext); }
=           							{ return yy::parser::make_EQUALS(
													std::make_unique<Node>("equals", yytext)); }
\<\(           							{ return yy::parser::make_SHELL_BEGIN(yytext); }
\(           							{ return yy::parser::make_LEFT_PARENTHESIS(yytext); }
\)           							{ return yy::parser::make_RIGHT_PARENTHESIS(yytext); }
;           							{ return yy::parser::make_SEMI(yytext); }
[ \t]+									{ return yy::parser::make_BLANK(yytext); }
<INITIAL,STRING>${NOT_SPECIAL}+			{ std::string text(yytext);
											return yy::parser::make_VAR(
													std::make_unique<VarNode>("VAREXP", text.substr(1))); }
'([^'\\]|\\.)*'							{ std::string text(yytext);
											return yy::parser::make_QUOTE(
											std::make_unique<QuoteNode>("QUOTED",
												text.substr(1, text.length()-2))); }
\"										{ BEGIN(STRING); }
<STRING>\"								{ BEGIN(INITIAL); }
<STRING>([^"$\\]|\\.)+					{ return yy::parser::make_QUOTE(
													std::make_unique<QuoteNode>("DBLQUOTE", yytext)); }
\"\"									{ return yy::parser::make_QUOTE(
													std::make_unique<QuoteNode>("DBLQUOTE", "")); }
\+											{ return yy::parser::make_PLUS(yytext); }
\-											{ return yy::parser::make_MIN(yytext); }
\*											{ return yy::parser::make_MUL(yytext); }
\/											{ return yy::parser::make_DIV(yytext); }
<<EOF>>    								   return yy::parser::make_END();
%%
