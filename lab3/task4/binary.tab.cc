// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


// First part of user declarations.

#line 37 "binary.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "binary.tab.hh"

// User implementation prologue.

#line 51 "binary.tab.cc" // lalr1.cc:412
// Unqualified %code blocks.
#line 10 "binary.yy" // lalr1.cc:413

  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;
  std::shared_ptr<Node> root;

#line 59 "binary.tab.cc" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif



// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
#line 126 "binary.tab.cc" // lalr1.cc:479

  /// Build a parser object.
  parser::parser ()
#if YYDEBUG
     :yydebug_ (false),
      yycdebug_ (&std::cerr)
#endif
  {}

  parser::~parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  inline
  parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  parser::symbol_number_type
  parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s)
  {
      switch (that.type_get ())
    {
      case 26: // command
        value.move< std::shared_ptr<CommandNode> > (that.value);
        break;

      case 28: // concat
        value.move< std::shared_ptr<ConcatNode> > (that.value);
        break;

      case 6: // WORD
      case 8: // VAR
      case 9: // QUOTE
      case 10: // EQUALS
      case 18: // DOUBLE
      case 20: // stream
      case 21: // optline
      case 22: // line
      case 23: // assignment
      case 24: // assignment_w_frontblank
      case 25: // pipeline
      case 27: // field
      case 29: // unit
      case 30: // plus_minus
      case 31: // mul_div
        value.move< std::shared_ptr<Node> > (that.value);
        break;

      case 3: // NEWL
      case 4: // PIPE
      case 5: // SEMI
      case 7: // BLANK
      case 11: // SHELL_BEGIN
      case 12: // PLUS
      case 13: // MIN
      case 14: // MUL
      case 15: // DIV
      case 16: // LEFT_PARENTHESIS
      case 17: // RIGHT_PARENTHESIS
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 26: // command
        value.copy< std::shared_ptr<CommandNode> > (that.value);
        break;

      case 28: // concat
        value.copy< std::shared_ptr<ConcatNode> > (that.value);
        break;

      case 6: // WORD
      case 8: // VAR
      case 9: // QUOTE
      case 10: // EQUALS
      case 18: // DOUBLE
      case 20: // stream
      case 21: // optline
      case 22: // line
      case 23: // assignment
      case 24: // assignment_w_frontblank
      case 25: // pipeline
      case 27: // field
      case 29: // unit
      case 30: // plus_minus
      case 31: // mul_div
        value.copy< std::shared_ptr<Node> > (that.value);
        break;

      case 3: // NEWL
      case 4: // PIPE
      case 5: // SEMI
      case 7: // BLANK
      case 11: // SHELL_BEGIN
      case 12: // PLUS
      case 13: // MIN
      case 14: // MUL
      case 15: // DIV
      case 16: // LEFT_PARENTHESIS
      case 17: // RIGHT_PARENTHESIS
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }


  template <typename Base>
  inline
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " (";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 26: // command
        yylhs.value.build< std::shared_ptr<CommandNode> > ();
        break;

      case 28: // concat
        yylhs.value.build< std::shared_ptr<ConcatNode> > ();
        break;

      case 6: // WORD
      case 8: // VAR
      case 9: // QUOTE
      case 10: // EQUALS
      case 18: // DOUBLE
      case 20: // stream
      case 21: // optline
      case 22: // line
      case 23: // assignment
      case 24: // assignment_w_frontblank
      case 25: // pipeline
      case 27: // field
      case 29: // unit
      case 30: // plus_minus
      case 31: // mul_div
        yylhs.value.build< std::shared_ptr<Node> > ();
        break;

      case 3: // NEWL
      case 4: // PIPE
      case 5: // SEMI
      case 7: // BLANK
      case 11: // SHELL_BEGIN
      case 12: // PLUS
      case 13: // MIN
      case 14: // MUL
      case 15: // DIV
      case 16: // LEFT_PARENTHESIS
      case 17: // RIGHT_PARENTHESIS
        yylhs.value.build< std::string > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 45 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<Node>("stream","");
                                yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ());
                                root = yylhs.value.as< std::shared_ptr<Node> > ();}
#line 573 "binary.tab.cc" // lalr1.cc:859
    break;

  case 3:
#line 48 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[2].value.as< std::shared_ptr<Node> > ();
                                yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ());
                                root = yylhs.value.as< std::shared_ptr<Node> > ();}
#line 581 "binary.tab.cc" // lalr1.cc:859
    break;

  case 4:
#line 53 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<Node>("optline","empty"); }
#line 587 "binary.tab.cc" // lalr1.cc:859
    break;

  case 5:
#line 54 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<Node>("optline","has line");
                         yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 594 "binary.tab.cc" // lalr1.cc:859
    break;

  case 6:
#line 58 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 600 "binary.tab.cc" // lalr1.cc:859
    break;

  case 7:
#line 59 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<Node>("line","");
                         yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[2].value.as< std::shared_ptr<Node> > ());
                         yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 608 "binary.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 64 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<AssignmentNode>(yystack_[4].value.as< std::shared_ptr<Node> > (), yystack_[2].value.as< std::shared_ptr<Node> > ());
                          yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 615 "binary.tab.cc" // lalr1.cc:859
    break;

  case 9:
#line 66 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<CommandNode> > (); }
#line 621 "binary.tab.cc" // lalr1.cc:859
    break;

  case 10:
#line 67 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[1].value.as< std::shared_ptr<CommandNode> > (); }
#line 627 "binary.tab.cc" // lalr1.cc:859
    break;

  case 11:
#line 70 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 633 "binary.tab.cc" // lalr1.cc:859
    break;

  case 12:
#line 71 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 639 "binary.tab.cc" // lalr1.cc:859
    break;

  case 13:
#line 74 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 645 "binary.tab.cc" // lalr1.cc:859
    break;

  case 14:
#line 75 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<Node>("pipeline","");
                          yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[2].value.as< std::shared_ptr<Node> > ());
                         yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 653 "binary.tab.cc" // lalr1.cc:859
    break;

  case 15:
#line 80 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<CommandNode> > () = std::make_unique<CommandNode>(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 659 "binary.tab.cc" // lalr1.cc:859
    break;

  case 16:
#line 81 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<CommandNode> > () = yystack_[2].value.as< std::shared_ptr<CommandNode> > ();
                         yylhs.value.as< std::shared_ptr<CommandNode> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 666 "binary.tab.cc" // lalr1.cc:859
    break;

  case 17:
#line 85 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<ConcatNode> > (); }
#line 672 "binary.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 86 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 678 "binary.tab.cc" // lalr1.cc:859
    break;

  case 19:
#line 89 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<ConcatNode> > () = std::make_unique<ConcatNode>("concatenate","");
                          yylhs.value.as< std::shared_ptr<ConcatNode> > ()->children.push_back(yystack_[1].value.as< std::shared_ptr<Node> > ());
                          yylhs.value.as< std::shared_ptr<ConcatNode> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 686 "binary.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 92 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<ConcatNode> > () = yystack_[1].value.as< std::shared_ptr<ConcatNode> > ();
                         yylhs.value.as< std::shared_ptr<ConcatNode> > ()->children.push_back(yystack_[0].value.as< std::shared_ptr<Node> > ()); }
#line 693 "binary.tab.cc" // lalr1.cc:859
    break;

  case 21:
#line 96 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 699 "binary.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 97 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 705 "binary.tab.cc" // lalr1.cc:859
    break;

  case 23:
#line 98 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 711 "binary.tab.cc" // lalr1.cc:859
    break;

  case 24:
#line 99 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 717 "binary.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 100 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 723 "binary.tab.cc" // lalr1.cc:859
    break;

  case 26:
#line 101 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<ShellNode>("SUBSHELL", ""); 
                                        yylhs.value.as< std::shared_ptr<Node> > ()->children.push_back(yystack_[1].value.as< std::shared_ptr<Node> > ());}
#line 730 "binary.tab.cc" // lalr1.cc:859
    break;

  case 27:
#line 103 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[1].value.as< std::shared_ptr<Node> > (); }
#line 736 "binary.tab.cc" // lalr1.cc:859
    break;

  case 28:
#line 106 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 742 "binary.tab.cc" // lalr1.cc:859
    break;

  case 29:
#line 107 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<MathNode>(MathNode::Op::PLUS, yystack_[2].value.as< std::shared_ptr<Node> > (), yystack_[0].value.as< std::shared_ptr<Node> > ());  }
#line 748 "binary.tab.cc" // lalr1.cc:859
    break;

  case 30:
#line 108 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<MathNode>(MathNode::Op::MIN, yystack_[2].value.as< std::shared_ptr<Node> > (), yystack_[0].value.as< std::shared_ptr<Node> > ());  }
#line 754 "binary.tab.cc" // lalr1.cc:859
    break;

  case 31:
#line 111 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = yystack_[0].value.as< std::shared_ptr<Node> > (); }
#line 760 "binary.tab.cc" // lalr1.cc:859
    break;

  case 32:
#line 112 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<MathNode>(MathNode::Op::MUL, yystack_[2].value.as< std::shared_ptr<Node> > (), yystack_[0].value.as< std::shared_ptr<Node> > ());  }
#line 766 "binary.tab.cc" // lalr1.cc:859
    break;

  case 33:
#line 113 "binary.yy" // lalr1.cc:859
    { yylhs.value.as< std::shared_ptr<Node> > () = std::make_unique<MathNode>(MathNode::Op::DIV, yystack_[2].value.as< std::shared_ptr<Node> > (), yystack_[0].value.as< std::shared_ptr<Node> > ());  }
#line 772 "binary.tab.cc" // lalr1.cc:859
    break;


#line 776 "binary.tab.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yysyntax_error_ (yystack_[0].state, yyla));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }


      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char parser::yypact_ninf_ = -6;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
      23,    12,     6,    21,    -6,    33,    -6,    -6,    37,    35,
       9,    -6,    -6,    23,    23,    23,     9,    -6,    -6,    -6,
      -6,    23,     9,    -6,    36,     9,    -6,    -5,    17,    -6,
      37,    -6,    -6,    -1,    -3,     6,    22,     9,     9,    22,
       9,     9,    -6,    -6,    -6,    17,    17,    -6,    -6
  };

  const unsigned char
  parser::yydefact_[] =
  {
       4,    15,     0,     0,     2,     5,    11,    13,     6,     9,
       0,    12,     1,     4,     0,     0,    10,    21,    22,    24,
      25,     4,     0,    23,     0,    17,    31,    18,    28,     3,
       7,    14,    16,     0,     0,     0,    20,     0,     0,    19,
       0,     0,    26,    27,     8,    29,    30,    32,    33
  };

  const signed char
  parser::yypgoto_[] =
  {
      -6,    24,    31,    -6,    -2,    32,    34,    -6,    30,    -6,
      -4,     1,     2
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     3,     4,     5,     6,     7,     8,     9,    24,    25,
      26,    27,    28
  };

  const unsigned char
  parser::yytable_[] =
  {
      11,    17,    13,    18,    19,    20,    21,    37,    38,    37,
      38,    22,     1,    23,    43,    17,    42,    18,    19,    20,
      21,    12,    10,    34,    13,    22,    36,    23,    39,     1,
       2,    40,    41,    44,    37,    38,    47,    48,    14,    45,
      46,    15,    16,    35,    29,    33,    32,    31,    30
  };

  const unsigned char
  parser::yycheck_[] =
  {
       2,     6,     3,     8,     9,    10,    11,    12,    13,    12,
      13,    16,     6,    18,    17,     6,    17,     8,     9,    10,
      11,     0,    10,    22,     3,    16,    25,    18,    27,     6,
       7,    14,    15,    35,    12,    13,    40,    41,     5,    37,
      38,     4,     7,     7,    13,    21,    16,    15,    14
  };

  const unsigned char
  parser::yystos_[] =
  {
       0,     6,     7,    20,    21,    22,    23,    24,    25,    26,
      10,    23,     0,     3,     5,     4,     7,     6,     8,     9,
      10,    11,    16,    18,    27,    28,    29,    30,    31,    21,
      25,    24,    27,    20,    30,     7,    30,    12,    13,    30,
      14,    15,    17,    17,    23,    31,    31,    29,    29
  };

  const unsigned char
  parser::yyr1_[] =
  {
       0,    19,    20,    20,    21,    21,    22,    22,    23,    23,
      23,    24,    24,    25,    25,    26,    26,    27,    27,    28,
      28,    29,    29,    29,    29,    29,    29,    29,    30,    30,
      30,    31,    31,    31
  };

  const unsigned char
  parser::yyr2_[] =
  {
       0,     2,     1,     3,     0,     1,     1,     3,     5,     1,
       2,     1,     2,     1,     3,     1,     3,     1,     1,     2,
       2,     1,     1,     1,     1,     1,     3,     3,     1,     3,
       3,     1,     3,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "NEWL", "PIPE", "SEMI",
  "WORD", "BLANK", "VAR", "QUOTE", "EQUALS", "SHELL_BEGIN", "PLUS", "MIN",
  "MUL", "DIV", "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS", "DOUBLE",
  "$accept", "stream", "optline", "line", "assignment",
  "assignment_w_frontblank", "pipeline", "command", "field", "concat",
  "unit", "plus_minus", "mul_div", YY_NULLPTR
  };


  const unsigned char
  parser::yyrline_[] =
  {
       0,    45,    45,    48,    53,    54,    58,    59,    64,    66,
      67,    70,    71,    74,    75,    80,    81,    85,    86,    89,
      92,    96,    97,    98,    99,   100,   101,   103,   106,   107,
     108,   111,   112,   113
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG



} // yy
#line 1080 "binary.tab.cc" // lalr1.cc:1167
