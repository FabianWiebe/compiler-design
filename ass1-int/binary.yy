%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
  #include <string>
  #include <memory>
  #include "Node.hh"
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  std::shared_ptr<Node> root;
}
%token <std::string> NEWL
%token <std::string> COMMA
%token <std::string> BLANK
%token <std::string> PLUS MINUS MUL DIV
%token <std::string> EQUALS
%token <std::string> OPENING_PARENTHESIS CLOSING_PARENTHESIS OPENING_CURLY_BRACKET CLOSING_CURLY_BRACKET OPENING_SQUARE_BRACKET CLOSING_SQUARE_BRACKET
%token <std::string> FOR DO END_KW REPEAT UNTIL IF THEN
%token <std::shared_ptr<ValueNode>> VALUE
%token <std::shared_ptr<Node>> WORD
%type <std::shared_ptr<Node>> stream
%type <std::shared_ptr<Node>> optline
%type <std::shared_ptr<Node>> line
%type <std::shared_ptr<Node>> command
%type <std::shared_ptr<Node>> unit
%type <std::shared_ptr<AssignmentNode>> assignment
%type <std::shared_ptr<Node>> plus_minus
%type <std::shared_ptr<Node>> mul_div
%token END 0 "end of file"
%%
stream : optline              { $$ = std::make_shared<Node>("stream","");
                                $$->children.push_back($1);
                                root = $$;}
       | stream NEWL optline  { $$ = $1;
                                $$->children.push_back($3);
                                root = $$;}
       ;

optline : /*empty*/   { $$ = std::make_shared<Node>("optline","empty"); }
        | line        { $$ = std::make_shared<Node>("optline","has line");
                         $$->children.push_back($1); }
        ;

line : command               { $$ = $1; }
      | BLANK command       { $$ = $2; }
      | BLANK               { $$ = std::make_shared<Node>("optline","empty"); }
      ;

command : WORD OPENING_PARENTHESIS plus_minus CLOSING_PARENTHESIS {
                      $$ = std::make_shared<CommandNode>($1->value);
                      $$->children.push_back($3); }

       | assignment    { $$ = $1; }
        ;

unit : WORD       { $$ = $1; }
       | VALUE    { $$ = $1; }
       | command    { $$ = $1; }
       | OPENING_PARENTHESIS plus_minus CLOSING_PARENTHESIS { $$ = $2; }
       ;

assignment : WORD EQUALS plus_minus { $$ = std::make_shared<AssignmentNode>($1, $3); }
            ;

plus_minus : mul_div { $$ = $1; }
      | plus_minus PLUS mul_div { $$ = std::make_shared<MathNode>($2, $1, $3); }
      | plus_minus MINUS mul_div { $$ = std::make_shared<MathNode>($2, $1, $3); }
      ;

mul_div : unit { $$ = $1; }
      | mul_div MUL unit { $$ = std::make_shared<MathNode>($2, $1, $3); }
      | mul_div DIV unit { $$ = std::make_shared<MathNode>($2, $1, $3); }
      ;
