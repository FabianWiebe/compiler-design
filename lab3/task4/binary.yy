%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
  #include "Node.hh"
  #include <string>
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  Node root;
}
%token <std::string> NEWL
%token <std::string> PIPE
%token <std::string> SEMI
%token <std::string> WORD
%token <std::string> BLANK
%token <std::string> VAR
%token <std::string> QUOTE
%token <std::string> DBLQUOTE
%token <std::string> EMPTYQUOTE
%token <std::string> EQUALS
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

assignment : unit EQUALS concat { $$ = Node("equals", $2);
                          $$.children.push_back($1);
                         $$.children.push_back($3); }
            | unit EQUALS unit { $$ = Node("equals", $2);
                          $$.children.push_back($1);
                         $$.children.push_back($3); }
            | assignment BLANK assignment { $$ = $1;
                         $$.children.push_back($3); }
            | command { $$ = $1; }
            ;

pipeline : assignment       { $$ = $1; }
      | pipeline PIPE assignment   { $$ = Node("pipeline","");
                          $$.children.push_back($1);
                         $$.children.push_back($3); }
      ;

command : field        { $$ = Node("command","");
                         $$.children.push_back($1); }
      | command BLANK field   { $$ = $1;
                         $$.children.push_back($3); }
      ;

field : concat        { $$ = $1; }
      | unit          { $$ = $1; }
      ;

concat : unit       { $$ = Node("concatenate","");
                          $$.children.push_back($1); }
     | concat unit   { $$ = $1;
                         $$.children.push_back($2); }
      ;

unit : WORD		{ $$ = Node("WORD", $1); }
       | VAR		{ $$ = Node("VAREXP", $1.substr(1));  }
       | QUOTE		{ $$ = Node("QUOTED", $1.substr(1, $1.length()-2)); }
       | DBLQUOTE	{ $$ = Node("DBLQUOTE", $1); }
       | EMPTYQUOTE  { $$ = Node("DBLQUOTE", ""); }
       | EQUALS { $$ = Node("equals", $1); }
       | SHELL_BEGIN stream SHELL_END { $$ = Node("SUBSHELL", ""); 
                                        $$.children.push_back($2);}
        | BLANK unit { $$ = $2; }
        | unit BLANK { $$ = $1; }
       ;
