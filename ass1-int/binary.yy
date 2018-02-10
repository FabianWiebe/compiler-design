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
%token <std::string> COMP
%token <std::shared_ptr<ValueNode>> VALUE
%token <std::shared_ptr<Node>> WORD
%type <std::shared_ptr<Node>> stream
%type <std::shared_ptr<Node>> optline
%type <std::shared_ptr<Node>> line
%type <std::shared_ptr<Node>> command
%type <std::shared_ptr<Node>> unit
%type <std::shared_ptr<AssignmentNode>> assignment
%type <std::shared_ptr<Node>> plus_minus
%type <std::shared_ptr<Node>> mul_div params
%type <std::string> opt_blank opt_newl
%token END 0 "end of file"
%%
stream : optline              { $$ = std::make_shared<Node>("stream","");
                                $$->children.push_back($1);
                                root = $$;}
       | stream NEWL optline  { $$ = $1;
                                $$->children.push_back($3);
                                root = $$;}
       ;

optline : opt_blank   { $$ = std::make_shared<Node>("optline","empty"); }
        | line        { $$ = std::make_shared<Node>("optline","has line");
                         $$->children.push_back($1); }
        ;

line : opt_blank command       { $$ = $2; }
      ;

command : WORD opt_blank OPENING_PARENTHESIS opt_blank params CLOSING_PARENTHESIS opt_blank {
                      $$ = std::make_shared<CommandNode>($1->value);
                      $$->children = $5->children; }
      | WORD opt_blank params {
                      $$ = std::make_shared<CommandNode>($1->value);
                      $$->children = $3->children; }
      | WORD opt_blank OPENING_PARENTHESIS opt_blank CLOSING_PARENTHESIS opt_blank {
                      $$ = std::make_shared<CommandNode>($1->value); }
      | IF opt_blank plus_minus THEN opt_blank stream END_KW opt_blank {
                      $$ = std::make_shared<IfNode>($3, $6); }
      | assignment    { $$ = $1; }
      ;



params : plus_minus              { $$ = std::make_shared<Node>("parameters",""); 
                                    $$->children.push_back($1); }
      | params COMMA opt_blank plus_minus { $$ = $1;
                                              $$->children.push_back($4); }
      ;

unit : WORD opt_blank      { $$ = $1; }
       | VALUE opt_blank   { $$ = $1; }
       | command          { $$ = $1; }
       | plus_minus COMP opt_blank plus_minus { $$ = std::make_shared<CompNode>($2, $1, $4); }
       | OPENING_PARENTHESIS opt_blank plus_minus CLOSING_PARENTHESIS opt_blank { $$ = $3; }
       ;

assignment : WORD opt_blank EQUALS opt_blank plus_minus {
                                    $$ = std::make_shared<AssignmentNode>($1, $5); }
            ;

plus_minus : mul_div { $$ = $1; }
      | plus_minus PLUS opt_blank mul_div { $$ = std::make_shared<MathNode>($2, $1, $4); }
      | plus_minus MINUS opt_blank mul_div { $$ = std::make_shared<MathNode>($2, $1, $4); }
      ;

mul_div : unit { $$ = $1; }
      | mul_div MUL opt_blank unit { $$ = std::make_shared<MathNode>($2, $1, $4); }
      | mul_div DIV opt_blank unit { $$ = std::make_shared<MathNode>($2, $1, $4); }
      ;

opt_blank : BLANK       { $$ = $1; }
          | /*empty*/   { $$ = ""; }
          ;

opt_newl : NEWL       { $$ = $1; }
          | /*empty*/   { $$ = ""; }
          ;