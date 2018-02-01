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
%token <std::string> BIT
%token <std::string> NEW_NUMBER
%type <std::string> number
%token END 0 "end of file"
%%
number : BIT               {                   total += $1[0]-(($1[0] >= 'a') ? 'a'-10:'0'); }
       | number BIT        { total = total*2; total += $2[0]-(($2[0] >= 'a') ? 'a'-10:'0'); }
       | number NEW_NUMBER       {std::cout << total << ' '; total = 0;}
       ;

