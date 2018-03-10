#ifndef LABTREES_H
#define LABTREES_H

/* DV1465 / DV1505 / DV1511 Lab-task example code.
   (C) Dr Andrew Moss, Erik Bergenholtz  2016, 2017, 2018
   This code is released into the public domain.

   You are free to use this code as a base for your second assignment after
   the lab sessions (it is not required that you do so). 

   2018: Took out the double-pointers.
*/
#include <list>
#include <set>
#include <initializer_list>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "Value.hh"
#include "Environment.hh"

void output_start_of_asm(std::ostream& stream) {
  stream << "  asm(" << std::endl;
}

void output_end_of_asm(std::ostream& stream, const std::list<std::string>& var_names) {
  stream << ":";
  if (!var_names.empty()) {
    auto itr = var_names.begin();
    stream << " [" << *itr << "] \"+g\" (" << *itr << ")";
    for (++itr; itr != var_names.end(); ++itr) {
      stream << "," << std::endl << "  [" << *itr << "] \"+g\" (" << *itr << ")";
    }
  }
  stream << R"(
:
: "rax", "rbx", "rdx", "cc"
  );
)";
}

void output_vars(std::ostream& stream, std::list<std::string>& var_names) {
  if (var_names.empty()) return;
  for (const std::string& var_name : var_names) {
    stream << "  std::cout << \"" << var_name << ": \" << " << var_name << " << std::endl;" << std::endl;
  }
}

bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}


/************* Three Address Instructions *************/
class ThreeAd
{
public:
        const std::string name, lhs, op, rhs;
        const Type l_type, r_type;

        ThreeAd(const std::string& name, const std::string& op, const std::string& lhs, std::string& rhs, const Type l_type, const Type r_type) :
                name(name), op(op), lhs(lhs), rhs(rhs), l_type(l_type), r_type(r_type)
        {
        }

        std::string format_value(const std::string& str) {
          if (is_digits(str)) {
            return "$" + str;
          }
          return "\%[" + str + "]";
        }

        void dump(std::ostream& stream = std::cout)
        {
                stream << "  /* Expand: " << name << " := ";
                stream << lhs << " " << op << " " << rhs << " */" << std::endl;
                if (op == "c") {
                  stream << "  " << name << " = " << lhs << ";" << std::endl;
                } else if (op == "==" || op == "!=") {
                  stream << "  if (" << lhs << " " << op << " " << rhs << ") " << std::endl;
                } else {
                  stream << "  " << name << " = " << lhs << " " << op << " " << rhs << ";" << std::endl;
                }

                return;
                stream << "\" movq " << format_value(lhs) << ", \%\%rax\\n\\t\"" << std::endl;
                stream << "\" movq " << format_value(rhs) << ", \%\%rbx\\n\\t\"" << std::endl;
                if (op == "c") {
                  stream << "/* copy is a dummy operation */" << std::endl;
                } else if (op == "+") {
                  stream << "\" addq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                } else if (op == "*") {
                  stream << "\" mulq \%\%rbx\\n\\t\"" << std::endl;
                } else if (op == "==" || op == "!=") {
                  stream << "\" subq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                } else {
                  stream << "/* not implemented case " << op << " */" << std::endl;
                }
                stream << "\" movq \%\%rax, " << format_value(name) << "\\n\\t\"" << std::endl << std::endl;
        }
        void dumpCFG(std::ostream& stream = std::cout)
        {
                if (op == "c") {
                  stream << "  " << name << " = " << lhs << ";\n";
                } else if (op == "==" || op == "!=") {
                  stream << "  if (" << lhs << " " << op << " " << rhs << ") \n";
                } else {
                  stream << "  " << name << " = " << lhs << " " << op << " " << rhs << ";\n";
                }
        }
};


/* Basic Blocks */
class BBlock
{
private:
        static int nCounter;
public:
        std::list<ThreeAd> instructions;
        BBlock *tExit, *fExit;
        std::string name;
        std::string cond_jump;

        BBlock() :
                tExit(NULL), fExit(NULL), name("blk" + std::to_string(nCounter++))
        {
        }

        void dump(std::ostream& stream = std::cout)
        {
                stream << "/* BBlock @ " << name << " */" << std::endl;
                //stream << "\"" << name << ":\\n\\t\"" << std::endl;
                stream << name << ":" << std::endl;
                if (instructions.empty()) {
                  //stream << "\" nop\\n\\t\"" << std::endl;
                }
                for(auto i : instructions) {
                        i.dump(stream);
                }
                stream << "/* True:    " << (tExit ? tExit->name : "0") << " */" << std::endl;
                if (tExit) {
                  //stream << "\" " << (cond_jump.empty() ? "jmp" : cond_jump) << " " << tExit->name << "\\n\\t\"" << std::endl;
                  stream << "  goto " << tExit->name << ";" << std::endl;
                }
                stream << "/* False:   " << (fExit ? fExit->name : "0")  << " */" << std::endl;
                if (fExit) {
                  //stream << "\" jmp " << fExit->name << "\\n\\t\"" << std::endl;
                  stream << "  else goto " << fExit->name << ";" << std::endl;
                }
        }
        void dumpCFG(std::ostream& stream = std::cout)
        {
                stream << name << " [shape=box, label=\"";
                for(auto i : instructions) {
                        i.dumpCFG(stream);
                }
                stream << "\"];" << std::endl;
                if (tExit) {
                  stream << name << " -> " << tExit->name << (fExit ? " [label=\"True\"];" : ";") << std::endl;
                }
                if (fExit) {
                  stream << name << "  -> " << fExit->name << " [label=\"False\"];" << std::endl;
                }
        }
};
int BBlock::nCounter = 0;

/******************** Expressions ********************/

std::ostream& indent(std::ostream& stream, int depth) {
  for (int i = 0; i < depth; ++i) {
    stream << " ";
  }
  return stream;
}

class Expression
{
private:
        static int tmp_counter;
public:
        const std::string name;

        Expression(const std::string& name) : name(name)
        {
        }
        virtual std::string makeNames(Environment& e, Type type) 
        {
          // Lecture 8 / slide 11.
          // Virtual (but not pure) to allow overriding in the leaves.
          auto str = "_t" + std::to_string(tmp_counter++);
          e.set(str, type);
          return str;
        }
        virtual std::pair<std::string, Type> convert(Environment& e, BBlock*) = 0; // Lecture 8 / slide 12.
        
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;

        virtual void assign_type(Environment & e, Type type) {
          throw std::invalid_argument( "Value is no left value" );
        }
};
int Expression::tmp_counter = 0;

class Math : public Expression
{
public:
        Expression *lhs, *rhs;

        Math(const std::string& op, Expression* lhs, Expression* rhs) :
                Expression(op), lhs(lhs), rhs(rhs)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          std::string left, right;
          Type left_type, right_type;
          std::tie(left, left_type) = lhs->convert(e, out);
          std::tie(right, right_type) = rhs->convert(e, out);
          Type type = left_type == Type::LONG && right_type == Type::LONG && name != "/" ? Type::LONG : Type::DOUBLE;
          auto gen_name = makeNames(e, type);
          out->instructions.emplace_back(gen_name, name, left, right, left_type, right_type);
          return {gen_name, type};
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << std::endl;
          lhs->dump(stream, depth+1);
          rhs->dump(stream, depth+1);
        }
};

class Var : public Expression
{
public:
        std::string var_name;
        Type type;

        Var(std::string var_name) :
                Expression("var"), var_name(var_name)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          if (type == Type::UNDEFINED) {
            type = e.get(var_name);
          }
          return {var_name, type};
        }

        virtual void assign_type(Environment & e, Type t) {
          type = t;
          e.set(var_name, t);
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << var_name << std::endl;
        }
};

class Constant : public Expression
{
public:
        Value value;

        Constant(Value value) :
                Expression("constant"), value(value)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          return {value.as_string(), value.type};
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << value << std::endl;
        }
};

class Comp : public Expression
{
public:
        Expression *lhs, *rhs;

        Comp(const std::string & op, Expression* lhs, Expression* rhs) :
                Expression(op), lhs(lhs), rhs(rhs)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          auto gen_name = makeNames(e, Type::BOOL);
          std::string left, right;
          Type left_type, right_type;
          std::tie(left, left_type) = lhs->convert(e, out);
          std::tie(right, right_type) = rhs->convert(e, out);
          out->instructions.emplace_back(gen_name, name, left, right, left_type, right_type);
          out->cond_jump = name == "==" ? "jz" : "jnz";
          return {gen_name, Type::BOOL};
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << std::endl;
          lhs->dump(stream, depth+1);
          rhs->dump(stream, depth+1);
        }
};


/******************** Statements ********************/
class Statement
{
public:
        const std::string name;

        Statement(const std::string& name) : name(name)
        {
        }
        virtual BBlock* convert(Environment& e, BBlock *) = 0;

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;
};


class Assignment : public Statement
{
public:
        Var *lhs;
        Expression *rhs;

        Assignment(std::string lhs, Expression *rhs) :
                Statement("A"), lhs(new Var(lhs)), rhs(rhs)
        {
        }

        BBlock* convert(Environment& e, BBlock *out)
        {
          std::string left, right;
          Type left_type, right_type;
          std::tie(right, right_type) = rhs->convert(e, out);
          lhs->assign_type(e, right_type);
          std::tie(left, left_type) = lhs->convert(e, out);
          out->instructions.emplace_back(left, "c", right, right, left_type, right_type);
          return out;
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          lhs->dump(stream, depth+1);
          rhs->dump(stream, depth+1);
        }
};

class Seq : public Statement
{
public:
        std::list<Statement*> statements;

        Seq(std::initializer_list<Statement*> statements) :
                Statement("S"), statements(statements)
        {
        }

        BBlock* convert(Environment& e, BBlock *out)
        {
          BBlock *current_out = out;
          for (Statement* statement : statements) {
            current_out = statement->convert(e, current_out);
          }
          return current_out;
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          for (Statement* statement : statements) {
            statement->dump(stream, depth+1);
          }
        }
};


class If : public Statement
{
public:
        Expression *condition;
        Statement *true_branch, *false_branch;

        If(Expression *condition, Statement *true_branch, Statement *false_branch = NULL) :
                Statement("I"), condition(condition), true_branch(true_branch), false_branch(false_branch)
        {
        }

        BBlock* convert(Environment& e, BBlock *out)
        {
          condition->convert(e, out);
          BBlock *true_block = new BBlock();
          BBlock *false_block = NULL;
          out->tExit = true_block;
          true_block = true_branch->convert(e, true_block);
          if (false_branch) {
            false_block = new BBlock();
            out->fExit = false_block;
            false_block = false_branch->convert(e, false_block);
          }
          BBlock *join_block = new BBlock();
          true_block->tExit = join_block;
          if (false_branch) {
            false_block->tExit = join_block;
          } else {
            out->fExit = join_block;
          }
          return join_block;
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          condition->dump(stream, depth+1);
          true_branch->dump(stream, depth+1);
          if (false_branch) {
            false_branch->dump(stream, depth+1);
          }
        }
};

class Loop : public Statement
{
public:
        Expression *condition;
        Statement *body;
        bool while_loop;

        Loop(Expression *condition, Statement *body, bool while_loop = true) :
                Statement(while_loop ? "while" : "do while"), condition(condition), body(body), while_loop(while_loop)
        {
        }

        BBlock* convert(Environment& e, BBlock *out)
        {
          if (while_loop) {
            BBlock *loop_header = new BBlock();
            out->tExit = loop_header;
            condition->convert(e, loop_header);
            BBlock *loop_body = new BBlock();
            loop_header->tExit = loop_body;
            loop_body = body->convert(e, loop_body);
            loop_body->tExit = loop_header;
            BBlock *join = new BBlock();
            loop_header->fExit = join;
            return join;
          } else { // do while
            BBlock *loop_body = new BBlock();
            out->tExit = loop_body;
            BBlock *loop_body_n = body->convert(e, loop_body);
            BBlock *loop_header = new BBlock();
            loop_body_n->tExit = loop_header;
            condition->convert(e, loop_header);
            loop_header->tExit = loop_body;
            BBlock *join = new BBlock();
            loop_header->fExit = join;
            return join;
          }
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          if (while_loop) {
            condition->dump(stream, depth+1);
            body->dump(stream, depth+1);
          } else {
            body->dump(stream, depth+1);
            condition->dump(stream, depth+1);
          }
        }
};


/* Test cases */
Statement *test = new Seq({
                          new Assignment(
                                  "x",
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          ),new If(
                                  new Comp("==",
                                          new Var("x"),
                                          new Constant(10l)
                                  ),new Assignment(
                                          "y",
                                          new Math("+",
                                                  new Var("x"),
                                                  new Constant(1l)
                                          )
                                  ), new Assignment(
                                          "y",
                                          new Math("*",
                                                  new Var("x"),
                                                  new Constant(2l)
                                          )
                                  )
                          ), new Assignment(
                                  "x",
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          )
});

Statement *test2 = new Seq({
                          new Assignment(
                                  "x",
                                  new Constant(0l)
                          ),new Assignment(
                                  "y",
                                  new Constant(0l)
                          ),new Assignment(
                                  "x",
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          ),new Assignment(
                                  "y",
                                  new Math("+",
                                          new Var("y"),
                                          new Constant(1l)
                                  )
                          ),new If(
                                  new Comp("==",
                                          new Var("x"),
                                          new Constant(0l)
                                  ),new If(
                                          new Comp("==",
                                                  new Var("y"),
                                                  new Constant(0l)
                                          ),new Assignment(
                                                  "x",
                                                  new Constant(1l)
                                          ), new Assignment(
                                                  "y",
                                                  new Constant(2l)
                                          )
                                  ), new Assignment(
                                          "y",
                                          new Constant(3l)
                                  )
                          )
});

Statement *test3 = new Seq({
                          new Assignment(
                                  "x",
                                  new Constant(1l)
                          ),new Assignment(
                                  "y",
                                  new Constant(10l)
                          ),new Loop(
                                  new Comp("!=",
                                          new Var("x"),
                                          new Var("y")
                                  ),
                                  new Assignment(
                                          "x",
                                          new Math("+",
                                                  new Var("x"),
                                                  new Constant(1l)
                                          )
                                  )
                          )
});


/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpASM(BBlock *start, std::ostream& stream = std::cout)
{
        std::set<BBlock *> done, todo;
        todo.insert(start);
        while(todo.size()>0)
        {
                // Pop an arbitrary element from todo set
                auto first = todo.begin();
                BBlock *next = *first;
                todo.erase(first);
                next->dump(stream);
                stream << std::endl;
                done.insert(next);
                if(next->tExit!=NULL && done.find(next->tExit)==done.end())
                        todo.insert(next->tExit);
                if(next->fExit!=NULL && done.find(next->fExit)==done.end())
                        todo.insert(next->fExit);
        }
}

/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpCFG(BBlock *start, std::ostream& stream = std::cout)
{
        stream << "digraph {" << std::endl;
        std::set<BBlock *> done, todo;
        todo.insert(start);
        while(todo.size()>0)
        {
                // Pop an arbitrary element from todo set
                auto first = todo.begin();
                BBlock *next = *first;
                todo.erase(first);
                next->dumpCFG(stream);
                done.insert(next);
                if(next->tExit!=NULL && done.find(next->tExit)==done.end())
                        todo.insert(next->tExit);
                if(next->fExit!=NULL && done.find(next->fExit)==done.end())
                        todo.insert(next->fExit);
        }
        stream << "}" << std::endl;
}

#endif
