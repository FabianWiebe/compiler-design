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
%token <std::string> PIPE
%token <std::string> SEMI
%token <std::shared_ptr<Node>> WORD
%token <std::string> BLANK
%token <std::shared_ptr<Node>> VAR
%token <std::shared_ptr<Node>> QUOTE
%token <std::shared_ptr<Node>> EQUALS
%token <std::string> SHELL_BEGIN
%token <std::string> SHELL_END
%token <std::string> PLUS
%token <std::string> MIN
%token <std::string> MUL
%token <std::string> DIV
%type <std::shared_ptr<Node>> stream
%type <std::shared_ptr<Node>> optline
%type <std::shared_ptr<Node>> line
%type <std::shared_ptr<Node>> pipeline
%type <std::shared_ptr<CommandNode>> command
%type <std::shared_ptr<ConcatNode>> concat
%type <std::shared_ptr<Node>> unit
%type <std::shared_ptr<Node>> field
%type <std::shared_ptr<Node>> assignment
%type <std::shared_ptr<Node>> assignment_w_frontblank
%type <std::shared_ptr<Node>> plus_minus
%type <std::shared_ptr<Node>> mul_div
%token END 0 "end of file"
%%
stream : optline              { $$ = std::make_unique<Node>("stream","");
                                $$->children.push_back($1);
                                root = $$;}
       | stream NEWL optline  { $$ = $1;
                                $$->children.push_back($3);
                                root = $$;}
       ;

optline : /*empty*/   { $$ = std::make_unique<Node>("optline","empty"); }
        | line        { $$ = std::make_unique<Node>("optline","has line");
                         $$->children.push_back($1); }
        ;

line : pipeline        { $$ = $1; }
     | line SEMI pipeline   { $$ = std::make_unique<Node>("line","");
                         $$->children.push_back($1);
                         $$->children.push_back($3); }
      ;

assignment : WORD EQUALS field BLANK assignment { $$ = std::make_unique<AssignmentNode>($1, $3);
                          $$->children.push_back($5); }
            | command { $$ = $1; }
            | command BLANK { $$ = $1; }
            ;

assignment_w_frontblank : assignment { $$ = $1; }
                        | BLANK assignment { $$ = $2; }
                        ;

pipeline : assignment_w_frontblank       { $$ = $1; }
      | pipeline PIPE assignment_w_frontblank   { $$ = std::make_unique<Node>("pipeline","");
                          $$->children.push_back($1);
                         $$->children.push_back($3); }
      ;

command : WORD        { $$ = std::make_unique<CommandNode>($1); }
      | command BLANK field   { $$ = $1;
                         $$->children.push_back($3); }
      ;

field : concat        { $$ = $1; }
      | unit          { $$ = $1; }
      ;

concat : unit unit       { $$ = std::make_unique<ConcatNode>("concatenate","");
                          $$->children.push_back($1);
                          $$->children.push_back($2); }
     | concat unit   { $$ = $1;
                         $$->children.push_back($2); }
      ;

unit : plus_minus		{ $$ = $1; }
       | VAR		{ $$ = $1; }
       | QUOTE		{ $$ = $1; }
       | EQUALS { $$ = $1; }
       | SHELL_BEGIN stream SHELL_END { $$ = std::make_unique<ShellNode>("SUBSHELL", ""); 
                                        $$->children.push_back($2);}
       ;

plus_minus : mul_div { $$ = $1; }
      | plus_minus PLUS mul_div { $$ = std::make_unique<MathNode>(MathNode::Op::PLUS, $1, $3);  }
      | plus_minus MIN mul_div { $$ = std::make_unique<MathNode>(MathNode::Op::MIN, $1, $3);  }
      ;

mul_div : WORD { $$ = $1; }
      | mul_div MUL WORD { $$ = std::make_unique<MathNode>(MathNode::Op::MUL, $1, $3);  }
      | mul_div DIV WORD { $$ = std::make_unique<MathNode>(MathNode::Op::DIV, $1, $3);  }
      ;
