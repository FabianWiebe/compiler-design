%top{
	#include <string>
	#include <memory>
	#include "binary.tab.hh"
	#include "Node.hh"
	#define YY_DECL yy::parser::symbol_type yylex()
}
%option noyywrap nounput batch noinput
/* %option debug */
FILE *yyin; // needed to read from file instead of standard in

NOT_SPECIAL [^\n| \t'"=<>()+\-*/0-9,%^#\[\]{}]

%%

for									{ return yy::parser::make_FOR(yytext); }
do									{ return yy::parser::make_DO(yytext); }
end									{ return yy::parser::make_END_KW(yytext); }
repeat								{ return yy::parser::make_REPEAT(yytext); }
until								{ return yy::parser::make_UNTIL(yytext); }
if									{ return yy::parser::make_IF(yytext); }
then								{ return yy::parser::make_THEN(yytext); }
else								{ return yy::parser::make_ELSE(yytext); }
return								{ return yy::parser::make_RETURN(yytext); }
function							{ return yy::parser::make_FUNCTION(yytext); }
"="           						{ return yy::parser::make_EQUALS(yytext); }
","           						{ return yy::parser::make_COMMA(yytext); }
"=="|"!="|">"|"<"|">="|"<="         { return yy::parser::make_COMP(yytext); }
"("           						{ return yy::parser::make_OPENING_PARENTHESIS(yytext); }
")"           						{ return yy::parser::make_CLOSING_PARENTHESIS(yytext); }
"["           						{ return yy::parser::make_OPENING_SQUARE_BRACKET(yytext); }
"]"           						{ return yy::parser::make_CLOSING_SQUARE_BRACKET(yytext); }
"{"           						{ return yy::parser::make_OPENING_CURLY_BRACKET(yytext); }
"}"           						{ return yy::parser::make_CLOSING_CURLY_BRACKET(yytext); }
[ \t\n\r]+							{ /* remove white spaces and new lines */ }
"+"									{ return yy::parser::make_PLUS(yytext); }
"-"									{ return yy::parser::make_MINUS(yytext); }
"*"									{ return yy::parser::make_MUL(yytext); }
"/"									{ return yy::parser::make_DIV(yytext); }
"^"									{ return yy::parser::make_POW(yytext); }
"%"									{ return yy::parser::make_MOD(yytext); }
"#"									{ return yy::parser::make_SIZE(yytext); }
'([^'\\]|\\.)*'|\"([^"\\]|\\.)*\"	{ std::string text(yytext);
										std::string value = text.substr(1, text.length()-2);
										return yy::parser::make_VALUE(
												std::make_shared<ValueNode>(value)); }
[0-9]*\.[0-9]+						{ std::string text(yytext);
										double value = std::stod(text);
										return yy::parser::make_VALUE(
												std::make_shared<ValueNode>(value)); }
[0-9]+								{ std::string text(yytext);
										int value = std::stoi(text);
										return yy::parser::make_VALUE(
												std::make_shared<ValueNode>(value)); }
"true"|"false"						{ std::string text(yytext);
										bool value = text == "true";
										return yy::parser::make_VALUE(
												std::make_shared<ValueNode>(value)); }
{NOT_SPECIAL}+						{ return yy::parser::make_WORD(
													std::make_shared<WordNode>(yytext)); }
<<EOF>>    							{ return yy::parser::make_END(); }

%%
