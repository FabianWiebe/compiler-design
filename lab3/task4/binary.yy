%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
  #include <string>
  #include "Node.hh"
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  Node root;
}
%token <std::string> NEWL
%token <std::string> PIPE
%token <std::string> SEMI
%token <Node> WORD
%token <std::string> BLANK
%token <Node> VAR
%token <Node> QUOTE
%token <Node> DBLQUOTE
%token <Node> EQUALS
%token <std::string> SHELL_BEGIN
%token <std::string> SHELL_END
%type <Node> stream
%type <Node> optline
%type <Node> line
%type <Node> pipeline
%type <Node> command
%type <Node> concat
%type <Node> unit
%type <Node> field
%type <Node> assignment
%type <Node> assignment_w_frontblank
%token END 0 "end of file"
%%
stream : optline              { $$ = Node("stream","");
                                $$.children.push_back($1);
                                root = $$;}
       | stream NEWL optline  { $$ = $1;
                                $$.children.push_back($3);
                                root = $$;}
       ;

optline : /*empty*/   { $$ = Node("optline","empty"); }
        | line        { $$ = Node("optline","has line");
                         $$.children.push_back($1); }
        ;

line : pipeline        { $$ = $1; }
     | line SEMI pipeline   { $$ = Node("line","");
                         $$.children.push_back($1);
                         $$.children.push_back($3); }
      ;

assignment : WORD EQUALS field BLANK assignment { $$ = $2;
                          $$.children.push_back($1);
                         $$.children.push_back($3);
                          $$.children.push_back($5); }
            | command { $$ = $1; }
            | command BLANK { $$ = $1; }
            ;

assignment_w_frontblank : assignment { $$ = $1; }
                        | BLANK assignment { $$ = $2; }
                        
                        ;

pipeline : assignment_w_frontblank       { $$ = $1; }
      | pipeline PIPE assignment_w_frontblank   { $$ = Node("pipeline","");
                          $$.children.push_back($1);
                         $$.children.push_back($3); }
      ;

command : WORD        { $$ = Node("command","");
                         $$.children.push_back($1); }
      | command BLANK field   { $$ = $1;
                         $$.children.push_back($3); }
      ;

field : concat        { $$ = $1; }
      | unit          { $$ = $1; }
      ;

concat : unit unit       { $$ = Node("concatenate","");
                          $$.children.push_back($1);
                          $$.children.push_back($2); }
     | concat unit   { $$ = $1;
                         $$.children.push_back($2); }
      ;

unit : WORD		{ $$ = $1; }
       | VAR		{ $$ = $1; }
       | QUOTE		{ $$ = $1; }
       | DBLQUOTE	{ $$ = $1; }
       | EQUALS { $$ = $1; }
       | SHELL_BEGIN stream SHELL_END { $$ = Node("SUBSHELL", ""); 
                                        $$.children.push_back($2);}
       ;
