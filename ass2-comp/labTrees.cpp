#include "labTrees.hh"

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

std::string get_print_parm(Type type) {
    switch(type) {
      case Type::LONG: {
        return "%ld";
      }
      case Type::DOUBLE: {
        return "%g"; //f
      }
      case Type::BOOL: {
        return "%d";
      }
    }
    return "";
}

std::list<Type> types{Type::LONG, Type::DOUBLE, Type::BOOL, Type::STRING};

void define_vars(std::ostream& stream, Environment& e) {
  for (Type type : types) {
    auto var_names = e.get_all_of_type(type);
    if (!var_names.empty()) {
      auto itr = var_names.begin();
      stream << "  " << type_as_string(type) << " " << *itr;
      for (++itr; itr != var_names.end(); ++itr) {
        stream << ", " << *itr;
      }
      stream << ";" << std::endl;
    }
  }
  for (auto& pair : e.get_const_values()) {
    Type type = pair.second.type;
    if (type == Type::ARRAY) type = Type::DOUBLE;
    stream << "  " << type_as_string(type) << " " << pair.first << "[] = " << pair.second << ";" << std::endl;
  }
}

void output_vars(std::ostream& stream, Environment& e) {

  for (Type type : types) { 
    auto var_names = e.get_all_of_type(type);
    for (const std::string& var_name : var_names) {
      switch(type) {
        case Type::STRING: {
          stream << "  printf(\"" << var_name << " = \");" << std::endl;
          stream << "  printf(var_name);" << std::endl;
          stream << "  printf(\\n);" << std::endl;
          break;
        }
        default: {
          stream << "  printf(\"" << var_name << " = " << get_print_parm(type) << "\\n\", " << var_name << ");" << std::endl;
        }
      }
    }
  }
}

bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

std::ostream& indent(std::ostream& stream, int depth) {
  for (int i = 0; i < depth; ++i) {
    stream << " ";
  }
  return stream;
}


int BBlock::nCounter = 0;
int Expression::tmp_counter = 0;


/* Test casesType:: */
Statement *test = new Seq({
                          new Assignment(
                                  new Var("x"),
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          ),new If(
                                  new Comp("==",
                                          new Var("x"),
                                          new Constant(10l)
                                  ),new Assignment(
                                          new Var("y"),
                                          new Math("+",
                                                  new Var("x"),
                                                  new Constant(1l)
                                          )
                                  ), new Assignment(
                                          new Var("y"),
                                          new Math("*",
                                                  new Var("x"),
                                                  new Constant(2l)
                                          )
                                  )
                          ), new Assignment(
                                  new Var("x"),
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          )
});

Statement *test2 = new Seq({
                          new Assignment(
                                  new Var("x"),
                                  new Constant(0l)
                          ),new Assignment(
                                  new Var("y"),
                                  new Constant(0l)
                          ),new Assignment(
                                  new Var("x"),
                                  new Math("+",
                                          new Var("x"),
                                          new Constant(1l)
                                  )
                          ),new Assignment(
                                  new Var("y"),
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
                                                  new Var("x"),
                                                  new Constant(1l)
                                          ), new Assignment(
                                                  new Var("y"),
                                                  new Constant(2l)
                                          )
                                  ), new Assignment(
                                          new Var("y"),
                                          new Constant(3l)
                                  )
                          )
});

Statement *test3 = new Seq({
                          new Assignment(
                                  new Var("x"),
                                  new Constant(1l)
                          ),new Assignment(
                                  new Var("y"),
                                  new Constant(10l)
                          ),new Loop(
                                  new Comp("!=",
                                          new Var("x"),
                                          new Var("y")
                                  ),
                                  new Assignment(
                                          new Var("x"),
                                          new Math("+",
                                                  new Var("x"),
                                                  new Constant(1l)
                                          )
                                  )
                          ),new CommandS("io.write",
                          {new Constant(std::string("x = ")),
                          new Var("x")}),
                          new CommandS("print",
                          {new Constant(std::string("\\ny =")),
                          new Var("y")})
});


/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpASM(Environment& e, BBlock *start, std::ostream& stream)
{
        stream << R"(#include "stdio.h"
#include "math.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
)";
        define_vars(stream, e);
        //output_start_of_asm(stream);

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

        //output_end_of_asm(stream, vars);
        //output_vars(stream, e);
        stream << "}" << std::endl;
}

/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpCFG(BBlock *start, std::ostream& stream)
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
