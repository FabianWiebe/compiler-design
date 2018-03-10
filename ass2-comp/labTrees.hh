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

void output_vars(std::ostream& stream, const std::list<std::string>& var_names) {
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

        ThreeAd(const std::string& name, const std::string& op, const std::string& lhs, std::string& rhs) :
                name(name), op(op), lhs(lhs), rhs(rhs)
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
                stream << "\" movq " << format_value(lhs) << ", \%\%rax\\n\\t\"" << std::endl;
                stream << "\" movq " << format_value(rhs) << ", \%\%rbx\\n\\t\"" << std::endl;
                if (op == "c") {
                  stream << "/* copy is a dummy operation */" << std::endl;
                } else if (op == "+") {
                  stream << "\" addq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                } else if (op == "*") {
                  stream << "\" mulq \%\%rbx\\n\\t\"" << std::endl;
                } else if (op == "=") {
                  stream << "\" subq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                } else {
                  stream << "/* not implemented case " << op << " */" << std::endl;
                }
                stream << "\" movq \%\%rax, " << format_value(name) << "\\n\\t\"" << std::endl << std::endl;
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
                stream << "\"" << name << ":\\n\\t\"" << std::endl;
                //stream << name << ":" << std::endl;
                if (instructions.empty()) {
                  stream << "\" nop\\n\\t\"" << std::endl;
                }
                for(auto i : instructions) {
                        i.dump(stream);
                }
                stream << "/* True:    " << (tExit ? tExit->name : "0") << " */" << std::endl;
                if (tExit) {
                  stream << "\" " << (cond_jump.empty() ? "jmp" : cond_jump) << " " << tExit->name << "\\n\\t\"" << std::endl;
                  //stream << "  goto " << tExit->name << ";" << std::endl;
                }
                stream << "/* False:   " << (fExit ? fExit->name : "0")  << " */" << std::endl;
                if (fExit) {
                  stream << "\" jmp " << fExit->name << "\\n\\t\"" << std::endl;
                  //stream << "  else goto " << fExit->name << ";" << std::endl;
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
        static std::set<std::string> var_names;
        const std::string name;

        Expression(const std::string& name) : name(name)
        {
        }
        virtual std::string makeNames() 
        {
          // Lecture 8 / slide 11.
          // Virtual (but not pure) to allow overriding in the leaves.
          auto str = "_t" + std::to_string(tmp_counter++);
          var_names.insert(str);
          return str;
        }
        virtual std::string convert(BBlock*) = 0; // Lecture 8 / slide 12.
        
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;
};
int Expression::tmp_counter = 0;
std::set<std::string> Expression::var_names;

class Math : public Expression
{
public:
        Expression *lhs, *rhs;
        const std::string op;

        Math(const std::string& op, Expression* lhs, Expression* rhs) :
                Expression(op), lhs(lhs), rhs(rhs)
        {
        }

        virtual std::string convert(BBlock* out)
        {
          auto gen_name = makeNames();
          auto left = lhs->convert(out);
          auto right = rhs->convert(out);
          out->instructions.emplace_back(gen_name, op, left, right);
          return gen_name;
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << std::endl;
          lhs->dump(stream, depth+1);
          rhs->dump(stream, depth+1);
        }
};

class Variable : public Expression
{
public:
        std::string var_name;

        Variable(std::string var_name) :
                Expression("var"), var_name(var_name)
        {
          var_names.insert(var_name);
        }

        virtual std::string makeNames() {
          return var_name;
        }

        virtual std::string convert(BBlock* out)
        {
          return makeNames();
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << var_name << std::endl;
        }
};

class Constant : public Expression
{
public:
        int value;

        Constant(int value) :
                Expression("constant"), value(value)
        {
        }

        virtual std::string makeNames()  {
          return std::to_string(value);
        }

        virtual std::string convert(BBlock* out)
        {
          return makeNames();
          
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << value << std::endl;
        }
};

class Comp : public Expression
{
public:
        Expression *lhs, *rhs;
        const std::string op;

        Comp(const std::string & op, Expression* lhs, Expression* rhs) :
                Expression(op), lhs(lhs), rhs(rhs)
        {
        }

        virtual std::string convert(BBlock* out)
        {
          auto gen_name = makeNames();
          auto left = lhs->convert(out);
          auto right = rhs->convert(out);
          out->instructions.emplace_back(gen_name, op, left, right);
          out->cond_jump = "jz";
          return gen_name;
          
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
        virtual BBlock* convert(BBlock *) = 0;

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;
};


class Assignment : public Statement
{
public:
        Variable *lhs;
        Expression *rhs;

        Assignment(std::string lhs, Expression *rhs) :
                Statement("A"), lhs(new Variable(lhs)), rhs(rhs)
        {
        }

        BBlock* convert(BBlock *out)
        {
          auto left = lhs->convert(out);
          auto right = rhs->convert(out);
          out->instructions.emplace_back(left, "c", right, right);
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

        BBlock* convert(BBlock *out)
        {
          BBlock *current_out = out;
          for (Statement* statement : statements) {
            current_out = statement->convert(current_out);
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

        BBlock* convert(BBlock *out)
        {
          condition->convert(out);
          BBlock *true_block = new BBlock();
          BBlock *false_block = NULL;
          out->tExit = true_block;
          true_block = true_branch->convert(true_block);
          if (false_branch) {
            false_block = new BBlock();
            out->fExit = false_block;
            false_block = false_branch->convert(false_block);
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


/* Test cases */
Statement *test = new Seq({
                          new Assignment(
                                  "x",
                                  new Math("+",
                                          new Variable("x"),
                                          new Constant(1)
                                  )
                          ),new If(
                                  new Comp("==",
                                          new Variable("x"),
                                          new Constant(10)
                                  ),new Assignment(
                                          "y",
                                          new Math("+",
                                                  new Variable("x"),
                                                  new Constant(1)
                                          )
                                  ), new Assignment(
                                          "y",
                                          new Math("*",
                                                  new Variable("x"),
                                                  new Constant(2)
                                          )
                                  )
                          ), new Assignment(
                                  "x",
                                  new Math("+",
                                          new Variable("x"),
                                          new Constant(1)
                                  )
                          )
});

Statement *test2 = new Seq({
                          new Assignment(
                                  "x",
                                  new Constant(0)
                          ),new Assignment(
                                  "y",
                                  new Constant(0)
                          ),new Assignment(
                                  "x",
                                  new Math("+",
                                          new Variable("x"),
                                          new Constant(1)
                                  )
                          ),new Assignment(
                                  "y",
                                  new Math("+",
                                          new Variable("y"),
                                          new Constant(1)
                                  )
                          ),new If(
                                  new Comp("==",
                                          new Variable("x"),
                                          new Constant(0)
                                  ),new If(
                                          new Comp("==",
                                                  new Variable("y"),
                                                  new Constant(0)
                                          ),new Assignment(
                                                  "x",
                                                  new Constant(1)
                                          ), new Assignment(
                                                  "y",
                                                  new Constant(2)
                                          )
                                  ), new Assignment(
                                          "y",
                                          new Constant(3)
                                  )
                          )
});

Statement *test3 = new Seq({
                          new Assignment(
                                  "x",
                                  new Constant(27)
                          ),new If(
                                  new Comp("==",
                                          new Variable("x"),
                                          new Constant(27)
                                  ),
                                  new Assignment(
                                          "y",
                                          new Math("+",
                                                  new Variable("x"),
                                                  new Math("*",
                                                            new Constant(2),
                                                            new Constant(3)
                                                  )
                                          )
                                  )
                          )
});


/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpCFG(BBlock *start, std::ostream& stream = std::cout)
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
