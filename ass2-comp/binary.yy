%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
  #include <string>
  #include <memory>
  #include "labTrees.hh"
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  Statement* root;
}
%error-verbose

%debug

%token <std::string> COMMA
%token <std::string> OPENING_PARENTHESIS CLOSING_PARENTHESIS OPENING_CURLY_BRACKET CLOSING_CURLY_BRACKET OPENING_SQUARE_BRACKET CLOSING_SQUARE_BRACKET
%token <std::string> FOR DO END_KW REPEAT UNTIL IF THEN ELSE RETURN FUNCTION
%token <Constant*> VALUE
%token <std::string> WORD
%type <Seq*> stream
%type <Statement*> statement assignment simple_assignment if
%type <Command*> command
%type <Expression*> expr l_expr
%type <Array*> params function_params l_params 
%token END 0 "end of file"

%left <std::string> EQUALS
%left <std::string> COMP
%left <std::string> PLUS MINUS
%left <std::string> MUL DIV MOD
%left <std::string> POW
%left <std::string> SIZE

%start stream

%%

stream : statement         { $$ = new Seq({$1}, "stream");
                             root = $$; }
       | stream statement  { $$ = $1;
                             $$->statements.push_back($2);
                             root = $$; }
       ;

if : IF expr THEN stream END_KW             { $$ = new If($2, $4); }
   | IF expr THEN stream ELSE stream END_KW { $$ = new If($2, $4, $6); }
   ;

statement : command               { $$ = new CommandS(*$1); }
          | if                    { $$ = $1; }
          | FOR WORD EQUALS expr COMMA expr DO stream END_KW
              { auto var = new Var($2);
                auto assignment = new Assignment(var, $4);
                auto cmp = new Comp("<=", var, $6);
                auto body = new Seq({$8, new Increment($2)}, "loop_body");
                auto loop = new Loop(cmp, body);
                $$ = new Seq({assignment, loop}, "for_loop"); }
          | REPEAT stream UNTIL expr { $$ = new Loop(new Not($4), $2, false); }
          | WORD params           { $$ = new CommandS($1, $2->expressions); } // command without parenthesis
          | simple_assignment     { $$ = $1; }
          | assignment            { $$ = $1; }
          | RETURN expr  { $$ = new Return($2); }
          | FUNCTION WORD OPENING_PARENTHESIS function_params CLOSING_PARENTHESIS stream END_KW
              { $$ = new Function($2, $4, $6); }
          ;

command : WORD OPENING_PARENTHESIS params CLOSING_PARENTHESIS
              { $$ = new Command($1, $3->expressions); }
        | WORD OPENING_PARENTHESIS CLOSING_PARENTHESIS
              { $$ = new Command($1, {}); }
        ;



params : expr              { $$ = new Array({$1}, "parameter values"); }
       | params COMMA expr { $$ = $1;
                             $$->expressions.push_back($3); }
       ;

l_params : l_expr                { $$ = new Array({$1}, "left expressions"); }
         | l_params COMMA l_expr { $$ = $1;
                                   $$->expressions.push_back($3); }
         ;

function_params : WORD                       { $$ = new Array({new Var($1)}, "function parameter names"); }
                | function_params COMMA WORD { $$ = $1;
                                               $$->expressions.push_back(new Var($3)); }
                ;

expr : l_expr             { $$ = $1; }
     | OPENING_PARENTHESIS expr CLOSING_PARENTHESIS { $$ = $2; }
     | OPENING_CURLY_BRACKET params CLOSING_CURLY_BRACKET
                          { $$ = $2; }
     | VALUE              { $$ = $1; }
     | command            { $$ = new CommandE(*$1); }
     | expr COMP expr     { $$ = new Math($2, $1, $3); }
     | expr PLUS expr     { $$ = new Math($2, $1, $3); }
     | expr MINUS expr    { $$ = new Math($2, $1, $3); }
     | expr MUL expr      { $$ = new Math($2, $1, $3); }
     | expr DIV expr      { $$ = new Math($2, $1, $3); }
     | expr POW expr      { $$ = new Math($2, $1, $3); }
     | expr MOD expr      { $$ = new Math($2, $1, $3); }
     | SIZE expr          { $$ = new Size($2); }
     ;

l_expr : WORD OPENING_SQUARE_BRACKET expr CLOSING_SQUARE_BRACKET
                          { $$ = new ArrayAccess($1, $3); }
       | WORD             { $$ = new Var($1); }
       ;


assignment : l_params EQUALS params { $$ = new Assignment($1, $3); }
           ;

simple_assignment : l_expr EQUALS expr { $$ = new Assignment($1, $3); }
                  ;
