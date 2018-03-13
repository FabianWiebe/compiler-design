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
#include <sstream>
#include "Value.hh"
#include "Environment.hh"

void output_start_of_asm(std::ostream& stream);

void output_end_of_asm(std::ostream& stream, const std::list<std::pair<std::string, Type>>& var_names);

std::string get_print_parm(Type type);

void define_vars(std::ostream& stream, Environment& e, bool esc_str = false);
void define_vars(std::ostream& stream, std::list<std::string>& var_names, Type type);

void output_vars(std::ostream& stream, Environment& e);
void output_vars(std::ostream& stream, std::list<std::pair<std::string, Type>>& vars);

bool is_digits(const std::string &str);


/************* Three Address Instructions *************/
class ThreeAd
{
public:
        const std::string name, lhs, op, rhs;
        const Type l_type, r_type, ret_type;

        ThreeAd(const std::string& name, const std::string& op, const std::string& lhs, const std::string& rhs, const Type l_type, const Type r_type, const Type ret_type) :
                name(name), op(op), lhs(lhs), rhs(rhs), l_type(l_type), r_type(r_type), ret_type(ret_type)
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
                if (op == "callE") {
                  if (lhs == "io.read") {
                    stream << "  if (scanf(\"%ld\", &"<< name << ") == EOF) exit(-1);" << std::endl;
                  } else { // function call
                    stream << "  " << (r_type != Type::UNDEFINED ? name + " = " : "") << lhs << "(";
                    if (l_type != Type::UNDEFINED) {
                      stream << rhs;
                    }
                    stream << ");" << std::endl;
                  }
                } else if (op == "callS") {
                   if (lhs == "print" || lhs == "io.write") {
                    stream << "  printf(\"";
                    if (l_type != Type::UNDEFINED) {
                      stream << get_print_parm(l_type);
                      if (r_type != Type::UNDEFINED) {
                        if (lhs == "print") stream << "\\t";
                        stream << get_print_parm(r_type);
                      }
                      if (lhs == "print") stream << "\\n";
                      stream << "\", " << rhs;
                      if (r_type != Type::UNDEFINED) {
                        stream << ", " << name;
                      }
                    } else {
                      if (lhs == "print") stream << "\\n";
                      stream << "\"";
                    }
                    stream << ");" << std::endl;
                  } else { // function call
                    stream << "  " << lhs << "(";
                    if (l_type != Type::UNDEFINED) {
                      stream << rhs;
                      if (r_type != Type::UNDEFINED) {
                        stream << ", " << name;
                      }
                    }
                    stream << ");" << std::endl;
                  }
                //} else if (op == "c[]") {
                  //stream << "  " << name << " = " << lhs << "[" << rhs << " - 1];" << std::endl;
                } else if (op == "return") {
                  stream << "  return " << lhs << ";" << std::endl;
                } else if (op == "#") {
                  stream << "  " << name << " = " << "sizeof(" << lhs << ") / sizeof(" << lhs << "[0]);" << std::endl;
                } else if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {  
                  stream << "  " << name << " = " << lhs << " " << op << (op == "/" ? "(double)" : "")<< " " << rhs << ";" << std::endl;
                } else {

                  bool double_op = op == "/" || l_type == Type::DOUBLE || r_type == Type::DOUBLE || ret_type == Type::DOUBLE;
                  std::set<std::pair<std::string, Type>> vars;
                  vars.emplace(name, ret_type);
                  if (!is_digits(lhs)) vars.emplace(lhs, l_type);
                  if (!is_digits(rhs)) vars.emplace(rhs, r_type);
                  output_start_of_asm(stream);
                    if (double_op) {
                      if (l_type == Type::DOUBLE) {
                        stream << "\" movsd " << format_value(lhs) << ", \%\%xmm0\\n\\t\"" << std::endl;                        
                      } else if (l_type == Type::LONG) {
                        stream << "\" movq " << format_value(lhs) << ", \%\%rax\\n\\t\"" << std::endl;
                        stream << "\" cvtsi2sdq \%\%rax, \%\%xmm0\\n\\t\"" << std::endl;
                      }
                      if (r_type == Type::DOUBLE) {
                        stream << "\" movsd " << format_value(rhs) << ", \%\%xmm1\\n\\t\"" << std::endl;                        
                      } else if (r_type == Type::LONG) {
                        stream << "\" movq " << format_value(rhs) << ", \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" cvtsi2sdq \%\%rbx, \%\%xmm1\\n\\t\"" << std::endl;
                      }
                      if (op == "c") {
                        stream << "/* copy is a dummy operation */" << std::endl;
                      } else if (op == "+") {
                        stream << "\" addsd \%\%xmm1, \%\%xmm0\\n\\t\"" << std::endl;
                      } else if (op == "-") {
                        stream << "\" subsd \%\%xmm1, \%\%xmm0\\n\\t\"" << std::endl;
                      } else if (op == "*") {
                        stream << "\" mulsd \%\%xmm1, \%\%xmm0\\n\\t\"" << std::endl;
                      } else if (op == "/") {
                        stream << "\" divsd \%\%xmm1, \%\%xmm0\\n\\t\"" << std::endl;
                      } else if (op == "c[]") {
                        // name = lhs [rhs]
                        stream << "\" dec \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" movq $8, \%\%rax\\n\\t\"" << std::endl;
                        stream << "\" mul \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" lea " << format_value(lhs) << ", \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" movsd (\%\%rax, \%\%rbx), \%\%xmm0\\n\\t\"" << std::endl;
                      } else if (op == "[]c") {
                        // name [rhs] = lhs
                        stream << "\" dec \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" movq $8, \%\%rax\\n\\t\"" << std::endl;
                        stream << "\" mul \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" lea " << format_value(name) << ", \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" movsd \%\%xmm0, (\%\%rax, \%\%rbx)\\n\\t\"" << std::endl;
                      } else {
                        stream << "/* not implemented case Type::" << op << " */" << std::endl;
                      }
                      if (ret_type != Type::ARRAY) stream << "\" movsd \%\%xmm0, " << format_value(name) << "\\n\\t\"" << std::endl << std::endl;

                    } else {
                      stream << "\" movq " << format_value(lhs) << ", \%\%rax\\n\\t\"" << std::endl;
                      stream << "\" movq " << format_value(rhs) << ", \%\%rbx\\n\\t\"" << std::endl;
                      if (op == "c") {
                        stream << "/* copy is a dummy operation */" << std::endl;
                      } else if (op == "+") {
                        stream << "\" addq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                      } else if (op == "-") {
                        stream << "\" subq \%\%rbx, \%\%rax\\n\\t\"" << std::endl;
                      } else if (op == "*") {
                        stream << "\" mul \%\%rbx\\n\\t\"" << std::endl;
                      } else if (op == "/") {
                        stream << "\" movq $0, \%\%rdx\\n\\t\"" << std::endl;
                        stream << "\" div \%\%rbx\\n\\t\"" << std::endl;
                      } else if (op == "%") {
                        stream << "\" movq $0, \%\%rdx\\n\\t\"" << std::endl;
                        stream << "\" div \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" movq \%\%rdx, \%\%rax\\n\\t\"" << std::endl;
                      } else if (op == "++") {
                        stream << "\" inc \%\%rax\\n\\t\"" << std::endl;
                      } else if (op == "!") {
                        stream << "\" xorq $1, \%\%rax\\n\\t\"" << std::endl;
                      }  else if (op == "^") {
                        stream << "\" movq \%\%rax, \%\%rcx\\n\\t\"" << std::endl;
                        stream << "\"pow_loop:\\n\\t\"" << std::endl;
                        stream << "\" cmp $1, \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" jbe pow_cont\\n\\t\"" << std::endl;
                        stream << "\" dec \%\%rbx\\n\\t\"" << std::endl;
                        stream << "\" mul \%\%rcx\\n\\t\"" << std::endl;
                        stream << "\" jmp pow_loop\\n\\t\"" << std::endl;
                        //stream << "\" call pow\\n\\t\"" << std::endl;
                        stream << "\"pow_cont:\\n\\t\"" << std::endl;
                      } else {
                        stream << "/* not implemented case Type::" << op << " */" << std::endl;
                      }
                      stream << "\" movq \%\%rax, " << format_value(name) << "\\n\\t\"" << std::endl << std::endl;
                    }
                  std::list<std::pair<std::string, Type>> vars_as_list(vars.begin(), vars.end());
                  output_end_of_asm(stream, vars_as_list);
                  //output_vars(stream, vars_as_list);
                }
        }
        std::string escape_quotes(const std::string& str) {
          std::string result = str;
          size_t pos = 0;
          while ((pos = result.find("\"", pos)) != std::string::npos) {
              result.insert(pos, "\\");
              pos += 2;
          }
          return result;
        }
        void dumpCFG(std::ostream& stream = std::cout)
        {
                std::string esc_str = "\\";
                stream << "  /* Expand: " << name << " := ";
                stream << lhs << " " << op << " " << rhs << " */" << std::endl;
                if (op == "c") {
                  stream << "  " << name << " = " << lhs << ";" << std::endl;
                } else if (op == "callE") {
                  if (lhs == "io.read") {
                    stream << "  if (scanf(" << esc_str << "\"%ld" << esc_str << "\", &"<< name << ") == EOF) exit(-1);" << std::endl;
                  } else { // function call
                    stream << "  " << (r_type != Type::UNDEFINED ? name + " = " : "") << lhs << "(";
                    if (l_type != Type::UNDEFINED) {
                      stream << rhs;
                    }
                    stream << ");" << std::endl;
                  }
                } else if (op == "callS") {
                   if (lhs == "print" || lhs == "io.write") {
                    stream << "  printf(" << esc_str << "\"";
                    if (l_type != Type::UNDEFINED) {
                      stream << get_print_parm(l_type);
                      if (r_type != Type::UNDEFINED) {
                        if (lhs == "print") stream << "\\t";
                        stream << get_print_parm(r_type);
                      }
                      if (lhs == "print") stream << esc_str << "\\n";
                      stream << esc_str << "\", " << rhs;
                      if (r_type != Type::UNDEFINED) {
                        stream << ", " << name;
                      }
                    } else {
                      if (lhs == "print") stream << esc_str << "\\n";
                      stream << esc_str << "\"";
                    }
                    stream << ");" << std::endl;
                  } else { // function call
                    stream << "  " << lhs << "(";
                    if (l_type != Type::UNDEFINED) {
                      stream << rhs;
                      if (r_type != Type::UNDEFINED) {
                        stream << ", " << name;
                      }
                    }
                    stream << ");" << std::endl;
                  }
                } else if (op == "^") {
                  stream << "  " << name << " = pow(" << lhs << ", " << rhs << ");" << std::endl;
                } else if (op == "++") {
                  stream << "  " << op << lhs << ";" << std::endl;
                } else if (op == "c[]") {
                  stream << "  " << name << " = " << lhs << "[" << rhs << " - 1];" << std::endl;
                } else if (op == "[]c") {
                  stream << "  " << name << "[" << rhs << " - 1] = " << lhs << ";" << std::endl;
                } else if (op == "!") {
                  stream << "  " << name << " = !" << lhs << ";" << std::endl;
                } else if (op == "return") {
                  stream << "  return " << lhs << ";" << std::endl;
                } else if (op == "#") {
                  stream << "  " << name << " = " << "sizeof(" << lhs << ") / sizeof(" << lhs << "[0]);" << std::endl;
                } else {  
                  stream << "  " << name << " = " << lhs << " " << op << (op == "/" ? "(double)" : "")<< " " << rhs << ";" << std::endl;
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
                for (auto i : instructions) {
                        i.dump(stream);
                }
                if (instructions.empty() && !tExit) {
                  stream << "  return 0;" << std::endl;
                }
                if (fExit) {
                  stream << "  if(" << instructions.back().name << ")" << std::endl;
                }
                stream << "  /* True:    " << (tExit ? tExit->name : "0") << " */" << std::endl;
                if (tExit) {
                  //stream << "\" " << (cond_jump.empty() ? "jmp" : cond_jump) << " " << tExit->name << "\\n\\t\"" << std::endl;
                  stream << "  goto " << tExit->name << ";" << std::endl;
                }
                stream << "  /* False:   " << (fExit ? fExit->name : "0")  << " */" << std::endl;
                if (fExit) {
                  //stream << "\" jmp " << fExit->name << "\\n\\t\"" << std::endl;
                  stream << "  else goto " << fExit->name << ";" << std::endl;
                }
        }
        void dumpCFG(std::ostream& stream = std::cout)
        {
                stream << name << " [shape=box, label=\"";
                for (auto i : instructions) {
                        i.dumpCFG(stream);
                }

                stream << "\"];" << std::endl;
                if (tExit) {
                  stream << name << " -> " << tExit->name << (fExit ? " [label=\"True\"];" : ";") << std::endl;
                }
                if (fExit) {
                  stream << name << " -> " << fExit->name << " [label=\"False\"];" << std::endl;
                }
        }
};

/******************** Expressions ********************/

std::ostream& indent(std::ostream& stream, int depth);

class Expression
{
private:
        static int tmp_counter;
public:
        const std::string name;

        Expression(const std::string& name = "uninitialized") : name(name)
        {
        }
        static std::string makeNames(Environment& e, Type type) 
        {
          // Lecture 8 / slide 11.
          // Virtual (but not pure) to allow overriding in the leaves.
          auto str = "_t" + std::to_string(tmp_counter++);
          e.set(str, type);
          return str;
        }
        static std::string makeNames() 
        {
          // Lecture 8 / slide 11.
          // Virtual (but not pure) to allow overriding in the leaves.
          return "_t" + std::to_string(tmp_counter++);
        }
        virtual std::pair<std::string, Type> convert(Environment& e, BBlock*) = 0; // Lecture 8 / slide 12.
        
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;

        virtual void assign_type(Environment & e, Type type) {
          throw std::invalid_argument( "Value is no left value" );
        }
        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          throw std::invalid_argument( "Node is no left node" );
        }
};

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
          out->instructions.emplace_back(gen_name, name, left, right, left_type, right_type, type);
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
                Expression("var"), var_name(var_name) {}

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

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          std::string name;
          std::tie(name, type) = value->convert(e, out);
          e.set(var_name, type);
          if (type != Type::ARRAY) {
            out->instructions.emplace_back(var_name, "c", name, name, type, type, type);
          } else {
            e.update_name(name, var_name);
          }
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << var_name << std::endl;
        }
};

class Size : public Expression
{
public:
        std::string array_name;

        Size(const std::string& array_name) :
                Expression("#"), array_name(array_name) {}

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          std::string name = makeNames(e, Type::LONG);
          out->instructions.emplace_back(name, "#", array_name, array_name, Type::ARRAY, Type::ARRAY, Type::LONG);
          return {name, Type::LONG};
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << array_name << std::endl;
        }
};

class ArrayAccess : public Expression
{
public:
        const std::string array_name;
        Expression* position;

        ArrayAccess(const std::string& array_name, Expression* position) :
                Expression("[]"), array_name(array_name), position(position)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          std::string name = makeNames(e, Type::DOUBLE);
          std::string pos_name = position->convert(e, out).first;
          out->instructions.emplace_back(name, "c[]", array_name, pos_name, Type::ARRAY, Type::LONG, Type::DOUBLE);
          return {name, Type::DOUBLE};
        }

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          std::string value_name = value->convert(e, out).first;
          std::string pos_name = position->convert(e, out).first;
          out->instructions.emplace_back(array_name, "[]c", value_name, pos_name, Type::DOUBLE, Type::LONG, Type::ARRAY);
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << array_name << name << std::endl;
          position->dump(stream, depth + 1);
        }
};

class Not : public Expression
{
public:
        Expression* bool_value;

        Not(Expression* bool_value) :
                Expression("!"), bool_value(bool_value)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          std::string name = makeNames(e, Type::LONG);
          std::string bool_name = bool_value->convert(e, out).first;
          out->instructions.emplace_back(name, "!", bool_name, bool_name, Type::LONG, Type::LONG, Type::LONG);
          return {name, Type::LONG};
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << std::endl;
          bool_value->dump(stream, depth + 1);
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
          if (value.type == Type::DOUBLE || value.type == Type::STRING) {
            std::string name = makeNames();
            e.store(name, value);
            return {name, value.type};
          }
          return {value.as_string(), value.type};
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << value << std::endl;
        }
};

class Array : public Expression
{
public:
        std::list<Expression*> expressions;

        Array(std::initializer_list<Expression*> expressions, const std::string& name = "array") :
                Expression(name), expressions(expressions)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          std::vector<Value> array;
          array.reserve(expressions.size());
          for (Expression* expression : expressions) {
            Constant* ptr = ptr = dynamic_cast<Constant*>(expression);
            if (!ptr) throw std::invalid_argument( "Non-constant in array." );
            array.push_back(ptr->value);
          }
          std::string name = makeNames(e, Type::ARRAY);
          Value array_value(array);
          e.store(name, array_value);
          return {name, Type::ARRAY};
        }

        virtual void assign_type(Environment & e, Type t) {
          //e.set(var_name, t);
        }

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          Array* ptr = dynamic_cast<Array*>(value);
          if (!ptr) throw std::invalid_argument( "Expression is not an array." );
          for (auto l_itr = expressions.begin(), r_itr = ptr->expressions.begin(); l_itr != expressions.end(); ++l_itr, ++r_itr) {
            (*l_itr)->assign(e, out, *r_itr);
          }
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "array" << std::endl;
          for (Expression* expression : expressions) {
            expression->dump(stream, depth+1);
          }
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
          auto gen_name = makeNames(e, Type::LONG);
          std::string left, right;
          Type left_type, right_type;
          std::tie(left, left_type) = lhs->convert(e, out);
          std::tie(right, right_type) = rhs->convert(e, out);
          out->instructions.emplace_back(gen_name, name, left, right, left_type, right_type, Type::LONG);
          if (name == "==") {
            out->cond_jump = "je";
          } else if (name == "!=") {
            out->cond_jump = "jne";
          } else if (name == "<") {
            out->cond_jump = "jl";
          } else if (name == ">") {
            out->cond_jump = "ja";
          } else if (name == "<=") {
            out->cond_jump = "jbe";
          } else if (name == ">=") {
            out->cond_jump = "jae";
          } 
          return {gen_name, Type::LONG};
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << std::endl;
          lhs->dump(stream, depth+1);
          rhs->dump(stream, depth+1);
        }
};

class Command
{
public:
        std::list<Expression*> parameters;
        std::string name;

        Command(const std::string& name, std::initializer_list<Expression*> parameters) :
                name(name), parameters(parameters) {}
        Command(const std::string& name, std::list<Expression*> parameters) :
                name(name), parameters(parameters) {}

        std::pair<std::string, Type> convert(Environment& e, BBlock *out, bool is_expression);
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          for (Expression* par : parameters) {
            par->dump(stream, depth+1);
          }
        }
};

class CommandE : public Expression, public Command
{
public:
  using Command::Command;
  CommandE(const Command& command) : Command(command) {}
  virtual std::pair<std::string, Type> convert(Environment& e, BBlock *out) {
    return Command::convert(e, out, true);
  }
  void dump(std::ostream& stream=std::cout, int depth = 0) {
    indent(stream, depth) << "Expression(" << Command::name << ")" << std::endl;
    Command::dump(stream, depth);
  }
};


/******************** Statements ********************/
class Statement
{
public:
        const std::string name;

        Statement(const std::string& name = "uninitialized") : name(name)
        {
        }
        virtual BBlock* convert(Environment& e, BBlock *) = 0;

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;
};

class CommandS : public Statement, public Command
{
public:
  using Command::Command;
  CommandS(const Command& command) : Command(command) {}
  BBlock* convert(Environment& e, BBlock *out) {
    Command::convert(e, out, false);
    return out;
  }
  void dump(std::ostream& stream=std::cout, int depth = 0) {
    indent(stream, depth) << "Statement(" << Command::name << ")" << std::endl;
    Command::dump(stream, depth);
  }
};



class Assignment : public Statement
{
public:
        Array *lhs;
        Array *rhs;

        Assignment(Array* lhs, Array *rhs) :
                Statement("complex Assignment"), lhs(lhs), rhs(rhs) {}
        Assignment(Expression* lhs, Expression *rhs) :
                Statement("simple Assignment"), lhs(new Array({lhs})), rhs(new Array({rhs})) {}

        BBlock* convert(Environment& e, BBlock *out)
        {
          if (lhs->expressions.size() == 1) {
            lhs->expressions.front()->assign(e, out, rhs->expressions.front());
          } else {
            Array* tmp_values = new Array({}, "tmp values");
            for (size_t i = 0; i < lhs->expressions.size(); ++i) {
              tmp_values->expressions.push_back(new Var(Expression::makeNames()));
            }
            tmp_values->assign(e, out, rhs);
            lhs->assign(e, out, tmp_values);
          }
          return out;
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          if (lhs->expressions.size() == 1) {
            lhs->expressions.front()->dump(stream, depth+1);
            rhs->expressions.front()->dump(stream, depth+1);
          } else {
            lhs->dump(stream, depth+1);
            rhs->dump(stream, depth+1);
          }
        }
};

class Seq : public Statement
{
public:
        std::list<Statement*> statements;

        Seq(std::initializer_list<Statement*> statements, const std::string& name = "S") :
                Statement(name), statements(statements)
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

class Increment : public Statement
{
public:
        const std::string var_name;

        Increment(const std::string& var_name) :
                Statement("++"), var_name(var_name)
        {
        }

        BBlock* convert(Environment& e, BBlock* out)
        {
          out->instructions.emplace_back(var_name, name, var_name, var_name, Type::LONG, Type::LONG, Type::LONG);
          return out;
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << var_name << std::endl;
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

class Function : public Statement
{
public:
        std::list<std::string> parameter_names;
        std::list<Type> parameter_types;
        Type return_type;
        BBlock* first_block;
        Statement *body;
        bool was_dumped;

        Function(const std::string& name, std::list<std::string>& parameter_names, Statement *body) :
                Statement(name), parameter_names(parameter_names), body(body), was_dumped(false)
        {
        }

        BBlock* convert(Environment& e, BBlock *out)
        {
          e.add_function(name, this);
          return out;
        }

        Type dump_function(Environment& e, const std::list<Type>& types) {
          if (was_dumped) {
            if (return_type == Type::UNDEFINED) return_type = e.get_return_type();
            if (return_type == Type::UNDEFINED) return_type = Type::LONG; // if recursion calls itself before a normal return
            return return_type;
          }
          was_dumped = true;
          parameter_types = types;
          first_block = new BBlock();
          BBlock* return_block = new BBlock();
          e.new_context(return_block);
          auto type_itr = types.begin();
          for (auto& name : parameter_names) {
            //e.set_function_parm(name, *type_itr++);
          }
          body->convert(e, first_block);
          return_type = e.clear_context();
          if (return_type == Type::UNDEFINED) return_type = Type::VOID;
          return return_type;
        }

        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Function(" << name << ")" << std::endl;
          if (!parameter_names.empty()) {
            indent(stream, depth) << "parameter_names: ";
            for (auto &parameter : parameter_names) {
              stream << parameter << ", ";
            }
            stream << std::endl;
          }
          body->dump(stream, depth+1);
        }
};

class Return : public Statement
{
public:
        Expression *expression;

        Return(Expression* expression) :
                Statement("return"), expression(expression) {}

        BBlock* convert(Environment& e, BBlock *out)
        {
          std::string name;
          Type type;
          std::tie(name, type) = expression->convert(e, out);
          e.set_return_type(type);
          out->instructions.emplace_back(name, "return", name, name, type, type, type);
          return out;
        }
        void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          expression->dump(stream, depth+1);
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


/* Test casesType:: */
extern Statement *test, *test2, *test3;


/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpASM(Environment& e, BBlock *start, std::ostream& stream = std::cout);

/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpCFG(Environment& e, BBlock *start, std::ostream& stream = std::cout);

#endif
