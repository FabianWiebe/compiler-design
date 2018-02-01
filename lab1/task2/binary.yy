%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code{
  unsigned int total = 0;
  #include <string>
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
}
%token <std::string> NL
%token <std::string> PIPE
%token <std::string> SEMI
%token <std::string> TEXT
%token <std::string> BLANK
%token <std::string> VAR
%token <std::string> QUOTE
%token <std::string> DBLQUOTE
%token <std::string> EMPTYQUOTE
%type <std::string> document
%token END 0 "end of file"
%%
document : 	// empty rule
       | document NL		{ std::cout << "NL" << std::endl; }
       | document PIPE		{ std::cout << "Pipe" << std::endl; }
       | document SEMI		{ std::cout << "Semi" << std::endl; }
       | document TEXT		{ std::cout << "Text ->" << $2 << "<-" << std::endl; }
       | document BLANK		{ std::cout << "Blank " << $2.length() << " chars" << std::endl; }
       | document VAR		{ std::cout << "Var ->" << $2.substr(1) << "<-" << std::endl; }
       | document QUOTE		{ std::cout << "Quoted ->" << $2.substr(1, $2.length()-2) << "<-" << std::endl; }
       | document DBLQUOTE	{ std::cout << "DblQuoted ->" << $2 << "<-" << std::endl; }
       | document EMPTYQUOTE  { std::cout << "DblQuoted -><-" << std::endl; }
       ;
