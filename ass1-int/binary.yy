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

%token <std::string> NEWL COMMA BLANK
%token <std::string> OPENING_PARENTHESIS CLOSING_PARENTHESIS OPENING_CURLY_BRACKET CLOSING_CURLY_BRACKET OPENING_SQUARE_BRACKET CLOSING_SQUARE_BRACKET
%token <std::string> FOR DO END_KW REPEAT UNTIL IF THEN ELSE RETURN FUNCTION
%token <std::shared_ptr<ValueNode>> VALUE
%token <std::shared_ptr<Node>> WORD
%type <std::shared_ptr<Node>> stream
%type <std::shared_ptr<Node>> optline
%type <std::shared_ptr<Node>> line
%type <std::shared_ptr<Node>> command
%type <std::shared_ptr<Node>> unit
%type <std::shared_ptr<AssignmentNode>> assignment simple_assignment
%type <std::shared_ptr<Node>> params function_params if l_params l_unit
%type <std::string> opt_newl
%token END 0 "end of file"

%left <std::string> EQUALS
%left <std::string> COMP
%left <std::string> PLUS MINUS
%left <std::string> MUL DIV MOD
%left <std::string> POW
%left <std::string> SIZE

%start stream

%%

stream : optline              { $$ = std::make_shared<Node>("stream","");
                                $$->children.push_back($1);
                                root = $$; }
       | stream NEWL optline  { $$ = $1;
                                $$->children.push_back($3);
                                root = $$; }
       ;

optline : /*empty*/           { $$ = std::make_shared<Node>("optline","empty"); }
        | line                { $$ = std::make_shared<Node>("optline","has line");
                                $$->children.push_back($1); }
        ;

if : IF opt_newl unit opt_newl THEN stream END_KW             { $$ = std::make_shared<IfNode>($3, $6); }
   | IF opt_newl unit opt_newl THEN stream ELSE stream END_KW { $$ = std::make_shared<IfNode>($3, $6);
                                                                $$->children.push_back($8); }
   ;

line : command               { $$ = $1; }
     | if                    { $$ = $1; }
     | FOR opt_newl simple_assignment COMMA opt_newl unit opt_newl DO stream END_KW
              { $$ = std::make_shared<Node>("for_loop", "");
                $$->children.push_back($3);
                auto incr = std::make_shared<IncrementNode>($3->children.front()->value);
                auto cmp = std::make_shared<CompNode>("<=", $3->children.front(), $6);
                auto body = std::make_shared<Node>("loop_body", "");
                body->children.push_back($9);
                body->children.push_back(incr);
                auto loop = std::make_shared<LoopNode>(cmp, body);
                $$->children.push_back(loop); }
     | REPEAT stream UNTIL opt_newl unit { auto neg_check = std::make_shared<NotNode>($5);
                                           $$ = std::make_shared<LoopNode>(neg_check, $2, false); }
     | WORD params           { $$ = std::make_shared<CommandNode>($1->value, $2); } // command without parenthesis
     | simple_assignment     { $$ = $1; }
     | assignment            { $$ = $1; }
     | RETURN opt_newl unit  { $$ = std::make_shared<ReturnNode>($3); }
     | RETURN                { $$ = std::make_shared<ReturnNode>(); }
     | FUNCTION WORD OPENING_PARENTHESIS opt_newl function_params opt_newl CLOSING_PARENTHESIS stream END_KW
              { $$ = std::make_shared<FunctionNode>($2->value, $5, $8); }
     ;

command : WORD OPENING_PARENTHESIS params CLOSING_PARENTHESIS
              { $$ = std::make_shared<CommandNode>($1->value, $3); }
        | WORD OPENING_PARENTHESIS CLOSING_PARENTHESIS
              { $$ = std::make_shared<CommandNode>($1->value); }
        ;



params : unit                                { $$ = std::make_shared<ArrayNode>(); 
                                               $$->children.push_back($1); }
       | params COMMA opt_newl unit { $$ = $1;
                                               $$->children.push_back($4); }
       ;

l_params : l_unit                         { $$ = std::make_shared<ArrayNode>(); 
                                            $$->children.push_back($1); }
         | l_params COMMA opt_newl l_unit { $$ = $1;
                                            $$->children.push_back($4); }
         ;

function_params : WORD                                { $$ = std::make_shared<ArrayNode>(); 
                                                        $$->children.push_back($1); }
                | function_params COMMA opt_newl WORD { $$ = $1;
                                                        $$->children.push_back($4); }
                ;

unit : l_unit             { $$ = $1; }
     | OPENING_PARENTHESIS unit CLOSING_PARENTHESIS { $$ = $2; }
     | OPENING_CURLY_BRACKET opt_newl params opt_newl CLOSING_CURLY_BRACKET
                          { $$ = std::make_shared<ArrayNode>();
                            $$->children = $3->children; }
     | VALUE              { $$ = $1; }
     | command            { $$ = $1; }
     | unit COMP opt_newl unit     { $$ = std::make_shared<CompNode>($2, $1, $4); }
     | unit PLUS opt_newl unit     { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | unit MINUS opt_newl unit    { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | unit MUL opt_newl unit      { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | unit DIV opt_newl unit      { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | unit POW opt_newl unit      { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | unit MOD opt_newl unit      { $$ = std::make_shared<MathNode>($2, $1, $4); }
     | SIZE unit          { $$ = std::make_shared<SizeNode>($2); }
     | simple_assignment  { $$ = $1; }
     ;

l_unit : WORD OPENING_SQUARE_BRACKET opt_newl unit opt_newl CLOSING_SQUARE_BRACKET
                          { $$ = std::make_shared<ArrayAccessNode>($1, $4); }
       | WORD             { $$ = $1; }
       ;


assignment : l_params EQUALS opt_newl params { $$ = std::make_shared<AssignmentNode>($1, $4); }
           ;

simple_assignment : l_unit EQUALS opt_newl unit { $$ = std::make_shared<AssignmentNode>($1, $4); }
                  ;

opt_newl : /*empty*/      { $$ = ""; }
         | NEWL           { $$ = $1; }
         ;

