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
%error-verbose

%debug

%token <std::string> NEWL
%token <std::string> COMMA
%token <std::string> BLANK
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
%type <std::shared_ptr<Node>> params
%token END 0 "end of file"

%left PLUS MINUS
%left MUL DIV
%left POW

%start stream

%%
stream : optline              { $$ = std::make_shared<Node>("stream","");
                                $$->children.push_back($1);
                                root = $$;}
       | stream NEWL optline  { $$ = $1;
                                $$->children.push_back($3);
                                root = $$;}
       ;

optline : /*empty*/  { $$ = std::make_shared<Node>("optline","empty"); }
        | line        { $$ = std::make_shared<Node>("optline","has line");
                         $$->children.push_back($1); }
        ;

line : command       { $$ = $1; }
      | IF unit THEN stream END_KW {
                      $$ = std::make_shared<IfNode>($2, $4); }
      | FOR assignment COMMA unit DO stream END_KW {
                      $$ = std::make_shared<Node>("For loop", "");
                      $$->children.push_back($2);
                      auto incr = std::make_shared<IncrementNode>($2->children.front()->value);
                      auto cmp = std::make_shared<CompNode>("<=", $2->children.front(), $4);
                      auto body = std::make_shared<Node>("loop body", "");
                      body->children.push_back($6);
                      body->children.push_back(incr);
                      auto loop = std::make_shared<LoopNode>(cmp, body);
                      $$->children.push_back(loop); }
      | WORD unit { $$ = std::make_shared<CommandNode>($1->value);
                      $$->children.push_back($2); }
      ;

command : WORD OPENING_PARENTHESIS params CLOSING_PARENTHESIS {
                      $$ = std::make_shared<CommandNode>($1->value);
                      $$->children = $3->children; }
      | WORD OPENING_PARENTHESIS CLOSING_PARENTHESIS {
                      $$ = std::make_shared<CommandNode>($1->value); }
      | assignment    { $$ = $1; }
      ;



params : unit              { $$ = std::make_shared<Node>("parameters",""); 
                                    $$->children.push_back($1); }
      | params COMMA unit { $$ = $1;
                                              $$->children.push_back($3); }
      ;

unit : WORD      { $$ = $1; }
        | OPENING_PARENTHESIS unit CLOSING_PARENTHESIS { $$ = $2; }
       | VALUE   { $$ = $1; }
       | command          { $$ = $1; }
       | unit COMP unit { $$ = std::make_shared<CompNode>($2, $1, $3); }
      | unit PLUS unit { $$ = std::make_shared<MathNode>("+", $1, $3); }
      | unit MINUS unit { $$ = std::make_shared<MathNode>("-", $1, $3); }
      | unit MUL unit { $$ = std::make_shared<MathNode>("*", $1, $3); }
      | unit DIV unit { $$ = std::make_shared<MathNode>("/", $1, $3); }
      | unit POW unit { $$ = std::make_shared<MathNode>("^", $1, $3); }
      ;

assignment : WORD EQUALS unit {
                                    $$ = std::make_shared<AssignmentNode>($1, $3); }
            ;