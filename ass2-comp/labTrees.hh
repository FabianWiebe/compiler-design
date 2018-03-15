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

std::string create_format_string(const std::list<Type>& types, const std::string& function);

bool is_digits(const std::string &str);

std::string get_jmp_code(const std::string& comp);

std::string combine(const std::list<std::string>& list, const std::string& delimeter = "");

std::string escape_new_lines(const std::string& str);


/************* Three Address Instructions *************/
class ThreeAd
{
public:
        const std::string name, lhs, op, rhs;
        const Type l_type, r_type, ret_type;
        const std::list<std::string> function_parameter_values;
        const std::list<Type> function_parameter_types;
        const std::list<std::pair<std::string, Type>> parms_for_stack;

        ThreeAd(const std::string& name, const std::string& op, const std::string& lhs, const std::string& rhs, const Type l_type, const Type r_type, const Type ret_type,
                const std::list<std::string>& function_parameter_values = std::list<std::string>(), const std::list<Type>& function_parameter_types = std::list<Type>(),
                const std::list<std::pair<std::string, Type>>& parms_for_stack = std::list<std::pair<std::string, Type>>()) :
                name(name), op(op), lhs(lhs), rhs(rhs), l_type(l_type), r_type(r_type), ret_type(ret_type),
                function_parameter_values(function_parameter_values), function_parameter_types(function_parameter_types), parms_for_stack(parms_for_stack) {}

        std::string format_value(const std::string& str) {
          if (is_digits(str)) {
            return "$" + str;
          }
          return str;
        }

        void push_parms_to_reg(std::ostream& stream, const std::list<std::string>& names, const std::list<Type>& types) {
          std::list<std::string> std_regs{"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
          std::list<std::string> dbl_regs{"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};
          auto std_reg_itr = std_regs.begin();
          auto dbl_reg_itr = dbl_regs.begin();
          auto types_itr = types.begin();
          size_t counter_dbl = 0, counter_std = 0;
          for (auto itr = names.begin(); itr != names.end(); ++itr, ++types_itr) {
            if (*types_itr == Type::DOUBLE) {
              if (dbl_reg_itr == dbl_regs.end()) throw std::invalid_argument( "Can only pass 8 doubles as parameters." );
              stream << "\t\tmovsd " << *itr << ", %" << *dbl_reg_itr++ << " # double Arg " << ++counter_dbl << std::endl;
            } else if (*types_itr == Type::STRING) {
              if (std_reg_itr == std_regs.end()) throw std::invalid_argument( "Can only pass 6 standard parameters. Pushing parameters on stack not implemented." );
              stream << "\t\tlea " << *itr << ", %" << *std_reg_itr++ << " # Arg " << ++counter_std << std::endl;
            } else {
              if (std_reg_itr == std_regs.end()) throw std::invalid_argument( "Can only pass 6 standard parameters. Pushing parameters on stack not implemented." );
              stream << "\t\tmovq " << format_value(*itr) << ", %" << *std_reg_itr++ << " # Arg " << ++counter_std << std::endl;
            }
          }
          stream << "\t\tmovq $" << counter_dbl <<", %rax # Vec args" << std::endl;
        }

        void dump(std::ostream& stream = std::cout)
        {
                bool is_comp = op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=";
                stream << "\t\t# Expand: " << name << " := ";
                if (op == "call") {
                  stream << op << " " << lhs << "(" << combine(function_parameter_values, ", ") << ") */" << std::endl;                 
                } else {
                  stream << lhs << " " << op << " " << rhs << std::endl;
                }

                if (op == "call") {
                  stream << "\t\tsubq $8, %rsp # Alignment" << std::endl;
                  if (lhs == "io.read") {
                    std::list<std::string> names{rhs, name};
                    std::list<Type> types{Type::STRING, Type::STRING}; // 2nd arg is a long address, but string will load the address
                    push_parms_to_reg(stream, names, types);
                    stream << "\t\tcall scanf" << std::endl;
                  } else if (lhs == "print" || lhs == "io.write") {
                    std::list<std::string> parms = function_parameter_values;
                    parms.push_front(rhs);
                    std::list<Type> types = function_parameter_types;
                    types.push_front(Type::STRING);
                    push_parms_to_reg(stream, parms, types);
                    stream << "\t\tcall printf" << std::endl;
                  } else { // function call
                    if (!parms_for_stack.empty()) {
                      stream << "\t\t# pushing function variables to stack " << std::endl;
                      for (auto & pair : parms_for_stack) {
                        stream << "\t\tpushq " << pair.first << " # " << type_as_string(pair.second) << std::endl;
                      }
                    }
                    push_parms_to_reg(stream, function_parameter_values, function_parameter_types);
                    stream << "\t\tcall " << lhs << std::endl;
                    if (!parms_for_stack.empty()) {
                      stream << "\t\t# popping function variables from stack " << std::endl;
                      for (auto itr = parms_for_stack.rbegin(); itr !=  parms_for_stack.rend(); ++itr) {
                        stream << "\t\tpopq " << itr->first << " # " << type_as_string(itr->second) << std::endl;
                      }
                    }
                    if (ret_type != Type::VOID) {
                      if (ret_type == Type::DOUBLE) {
                        stream << "\t\tmovsd %xmm0, " << name << " # get return value" << std::endl;
                      } else {
                        stream << "\t\tmovq %rax, " << name << " # get return value" << std::endl;
                      }
                    }
                  }
                  stream << "\t\taddq $8, %rsp # Alignment" << std::endl << std::endl;
                } else if (op == "return") {
                  if (ret_type != Type::VOID) {
                    if (ret_type == Type::DOUBLE) {
                      stream << "\t\tmovsd " << lhs << ", %xmm0 # return value" << std::endl;
                    } else {
                      stream << "\t\tmovq " << format_value(lhs) << ", %rax # return value" << std::endl;
                    }
                  }
                  stream << "\t\tret" << std::endl;
                } else {

                  bool double_op = op == "/" || l_type == Type::DOUBLE || r_type == Type::DOUBLE || ret_type == Type::DOUBLE;
                    if (double_op) {
                      if (l_type == Type::DOUBLE) {
                        stream << "\t\tmovsd " << lhs << ", %xmm0" << std::endl;                        
                      } else if (l_type == Type::LONG) {
                        stream << "\t\tmovq " << format_value(lhs) << ", %rax" << std::endl;
                        stream << "\t\tcvtsi2sdq %rax, %xmm0" << std::endl;
                      }
                      if (r_type == Type::DOUBLE) {
                        stream << "\t\tmovsd " << rhs << ", %xmm1" << std::endl;                        
                      } else if (r_type == Type::LONG) {
                        stream << "\t\tmovq " << format_value(rhs) << ", %rcx" << std::endl;
                        stream << "\t\tcvtsi2sdq %rcx, %xmm1" << std::endl;
                      }
                      if (op == "c") {
                        stream << "\t\t# copy is a dummy operation" << std::endl;
                      } else if (op == "+") {
                        stream << "\t\taddsd %xmm1, %xmm0" << std::endl;
                      } else if (op == "-") {
                        stream << "\t\tsubsd %xmm1, %xmm0" << std::endl;
                      } else if (op == "*") {
                        stream << "\t\tmulsd %xmm1, %xmm0" << std::endl;
                      } else if (op == "/") {
                        stream << "\t\tdivsd %xmm1, %xmm0" << std::endl;
                      } else if (op == "c[]") {
                        // name = lhs [rhs]
                        //stream << "\t\tdec %rcx" << std::endl;
                        stream << "\t\tmovq $8, %rax" << std::endl;
                        stream << "\t\tmul %rcx" << std::endl;
                        stream << "\t\tlea " << format_value(lhs) << ", %rcx" << std::endl;
                        stream << "\t\tmovsd (%rax, %rcx), %xmm0" << std::endl;
                      } else if (op == "[]c") {
                        // name [rhs] = lhs
                        //stream << "\t\tdec %rcx" << std::endl;
                        stream << "\t\tmovq $8, %rax" << std::endl;
                        stream << "\t\tmul %rcx" << std::endl;
                        stream << "\t\tlea " << format_value(name) << ", %rcx" << std::endl;
                        stream << "\t\tmovsd %xmm0, (%rax, %rcx)" << std::endl;
                      } else if (is_comp) {  
                        stream << "\t\tcomisd %xmm1, %xmm0" << std::endl; //cmppd
                      } else {
                        stream << "/* not implemented case Type::" << op << " */" << std::endl;
                      }
                      if (ret_type != Type::ARRAY && !is_comp) stream << "\t\tmovsd %xmm0, " << name << std::endl;
                      stream << std::endl;

                    } else {
                      if (l_type != Type::ARRAY) stream << "\t\tmovq " << format_value(lhs) << ", %rax" << std::endl;
                      if (r_type != Type::ARRAY) stream << "\t\tmovq " << format_value(rhs) << ", %rcx" << std::endl;
                      if (op == "c") {
                        stream << "\t\t# copy is a dummy operation" << std::endl;
                      } else if (op == "+") {
                        stream << "\t\taddq %rcx, %rax" << std::endl;
                      } else if (op == "-") {
                        stream << "\t\tsubq %rcx, %rax" << std::endl;
                      } else if (op == "*") {
                        stream << "\t\tmul %rcx" << std::endl;
                      } else if (op == "/") {
                        stream << "\t\tmovq $0, %rdx" << std::endl;
                        stream << "\t\tdiv %rcx" << std::endl;
                      } else if (op == "%") {
                        stream << "\t\tmovq $0, %rdx" << std::endl;
                        stream << "\t\tdiv %rcx" << std::endl;
                        stream << "\t\tmovq %rdx, %rax" << std::endl;
                      } else if (op == "++") {
                        stream << "\t\tinc %rax" << std::endl;
                      } else if (op == "!") {
                        stream << "\t\txorq $1, %rax" << std::endl;
                      }  else if (op == "^") {
                        stream << "\t\tmovq %rax, %rdi" << std::endl;
                        stream << "\t\tmovq %rcx, %rsi" << std::endl;
                        stream << "\t\tcall pow" << std::endl;
                      } else if (op == "#") {
                        stream << "\t\tmovq " << lhs << ", %rax" << std::endl;
                      } else if (is_comp) {  
                        stream << "\t\tcmp %rcx, %rax" << std::endl;
                      } else {
                        stream << "/* not implemented case Type::" << op << " */" << std::endl;
                      }
                      stream << "\t\tmovq %rax, " << name << std::endl << std::endl;
                    }
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
                stream << "  /* Expand: " << name << " := ";
                if (op == "call") {
                  stream << op << " " << lhs << "(" << combine(function_parameter_values, ", ") << ") */" << std::endl;                 
                } else {
                  stream << lhs << " " << op << " " << rhs << " */" << std::endl;
                }
                if (op == "c") {
                  stream << "  " << name << " = " << lhs << ";" << std::endl;
                } else if (op == "call") {
                  if (lhs == "io.read") {
                    stream << "  if (scanf(\\\"%ld\\\", &"<< name << ") == EOF) exit(-1);" << std::endl;
                  } else if (lhs == "print" || lhs == "io.write") {
                    std::list<std::string> parms = function_parameter_values;
                    parms.push_front(std::string("\\\"") + escape_new_lines(create_format_string(function_parameter_types, lhs)) + "\\\"");
                    stream << "  printf(" << combine(parms, ", ") << ");" << std::endl;
                  } else { // function call
                    if (!parms_for_stack.empty()) {
                      stream << "  /* pushing function variables to stack: ";
                      for (auto & pair : parms_for_stack) {
                        stream << type_as_string(pair.second) << " " << pair.first << ", ";
                      }
                      stream << "*/" << std::endl;
                    }
                    std::string save_return = "";
                    if (ret_type != Type::VOID) save_return = name + " = ";
                    stream << "  " << save_return << lhs << "(" << combine(function_parameter_values, ", ") << ");" << std::endl;
                  }
                } else if (op == "^") {
                  stream << "  " << name << " = pow(" << lhs << ", " << rhs << ");" << std::endl;
                } else if (op == "++") {
                  stream << "  " << op << lhs << ";" << std::endl;
                } else if (op == "c[]") {
                  stream << "  " << name << " = " << lhs << "[" << rhs << "];" << std::endl;
                } else if (op == "[]c") {
                  stream << "  " << name << "[" << rhs << "] = " << lhs << ";" << std::endl;
                } else if (op == "!") {
                  stream << "  " << name << " = !" << lhs << ";" << std::endl;
                } else if (op == "return") {
                  stream << "  return " << lhs << ";" << std::endl;
                } else if (op == "#") {
                  stream << "  " << name << " = (long) " << lhs << "[0];" << std::endl;
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
                stream << name << ":" << std::endl;
                for (auto i : instructions) {
                        i.dump(stream);
                }
                if (instructions.empty() && !tExit) {
                  stream << "\t\tret" << std::endl;
                }
                if (tExit) {
                  stream << "\t\t" << (cond_jump.empty() ? "jmp" : get_jmp_code(cond_jump)) << " " << tExit->name << " # True" << std::endl;
                }
                if (fExit) {
                  stream << "\t\tjmp " << fExit->name << " # False" << std::endl;
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
          auto str = "_t" + std::to_string(tmp_counter++) + e.get_current_function_name();
          e.set(str, type);
          return str;
        }
        static std::string makeNames(Environment& e) 
        {
          // Lecture 8 / slide 11.
          // Virtual (but not pure) to allow overriding in the leaves.
          return "_t" + std::to_string(tmp_counter++) + e.get_current_function_name();
        }
        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out) = 0; // Lecture 8 / slide 12.
        
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) = 0;

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
          if (name == "^") e.pow_used = true;
          std::string left, right;
          Type left_type, right_type;
          std::tie(left, left_type) = lhs->convert(e, out);
          std::tie(right, right_type) = rhs->convert(e, out);
          Type type = left_type == Type::LONG && right_type == Type::LONG && name != "/" ? Type::LONG : Type::DOUBLE;
          auto gen_name = makeNames(e, type);
          out->instructions.emplace_back(gen_name, name, left, right, left_type, right_type, type);
          return {gen_name, type};
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
        bool updated_name;

        Var(std::string var_name, bool updated_name = false) :
                Expression("var"), var_name(var_name), updated_name(updated_name) {}

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          if (!updated_name) var_name += e.get_current_function_name();
          updated_name = true;
          if (type == Type::UNDEFINED) {
            type = e.get(var_name);
          }
          return {var_name, type};
        }

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          if (!updated_name) var_name += e.get_current_function_name();
          updated_name = true;
          std::string name;
          std::tie(name, type) = value->convert(e, out);
          e.set(var_name, type);
          if (type != Type::ARRAY) {
            out->instructions.emplace_back(var_name, "c", name, name, type, type, type);
          } else {
            e.update_name(name, var_name);
          }
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
          array_name += e.get_current_function_name();
          std::string name = makeNames(e, Type::LONG);
          out->instructions.emplace_back(name, "#", array_name, array_name, Type::ARRAY, Type::ARRAY, Type::LONG);
          return {name, Type::LONG};
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << name << array_name << std::endl;
        }
};

class ArrayAccess : public Expression
{
public:
        std::string array_name;
        Expression* position;
        bool updated_name = false;

        ArrayAccess(const std::string& array_name, Expression* position) :
                Expression("[]"), array_name(array_name), position(position)
        {
        }

        virtual std::pair<std::string, Type> convert(Environment& e, BBlock* out)
        {
          if (!updated_name) array_name += e.get_current_function_name();
          updated_name = true;
          std::string name = makeNames(e, Type::DOUBLE);
          std::string pos_name = position->convert(e, out).first;
          out->instructions.emplace_back(name, "c[]", array_name, pos_name, Type::ARRAY, Type::LONG, Type::DOUBLE);
          return {name, Type::DOUBLE};
        }

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          if (!updated_name) array_name += e.get_current_function_name();
          updated_name = true;
          std::string value_name = value->convert(e, out).first;
          std::string pos_name = position->convert(e, out).first;
          out->instructions.emplace_back(array_name, "[]c", value_name, pos_name, Type::DOUBLE, Type::LONG, Type::ARRAY);
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
            std::string name = makeNames(e);
            e.store(name, value);
            return {name, value.type};
          }
          return {value.as_string(), value.type};
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual void assign(Environment & e, BBlock* out, Expression* value) {
          Array* ptr = dynamic_cast<Array*>(value);
          if (!ptr) throw std::invalid_argument( "Expression is not an array." );
          for (auto l_itr = expressions.begin(), r_itr = ptr->expressions.begin(); l_itr != expressions.end(); ++l_itr, ++r_itr) {
            (*l_itr)->assign(e, out, *r_itr);
          }
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
          out->cond_jump = name;
          return {gen_name, Type::LONG};
        }
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
  virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
  virtual BBlock* convert(Environment& e, BBlock *out) {
    Command::convert(e, out, false);
    return out;
  }
  virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual BBlock* convert(Environment& e, BBlock *out)
        {
          if (lhs->expressions.size() == 1) {
            lhs->expressions.front()->assign(e, out, rhs->expressions.front());
          } else {
            Array* tmp_values = new Array({}, "tmp values");
            for (size_t i = 0; i < lhs->expressions.size(); ++i) {
              tmp_values->expressions.push_back(new Var(Expression::makeNames(e), true));
            }
            tmp_values->assign(e, out, rhs);
            lhs->assign(e, out, tmp_values);
          }
          return out;
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual BBlock* convert(Environment& e, BBlock *out)
        {
          BBlock *current_out = out;
          for (Statement* statement : statements) {
            current_out = statement->convert(e, current_out);
          }
          return current_out;
        }
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
          indent(stream, depth) << "Statement(" << name << ")" << std::endl;
          for (Statement* statement : statements) {
            statement->dump(stream, depth+1);
          }
        }
};

class Increment : public Statement
{
public:
        std::string var_name;

        Increment(const std::string& var_name) :
                Statement("++"), var_name(var_name)
        {
        }

        virtual BBlock* convert(Environment& e, BBlock* out)
        {
          var_name += e.get_current_function_name();
          out->instructions.emplace_back(var_name, name, var_name, var_name, Type::LONG, Type::LONG, Type::LONG);
          return out;
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual BBlock* convert(Environment& e, BBlock *out)
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
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
          for (auto& parm_name : this->parameter_names) {
            parm_name += "_" + name;
          }
        }

        virtual BBlock* convert(Environment& e, BBlock *out)
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
          e.new_context(return_block, name);
          auto type_itr = types.begin();
          for (auto& name : parameter_names) {
            e.set_function_parm(name, *type_itr++);
          }
          body->convert(e, first_block);
          return_type = e.clear_context();
          if (return_type == Type::UNDEFINED) return_type = Type::VOID;
          return return_type;
        }

        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual BBlock* convert(Environment& e, BBlock *out)
        {
          std::string name;
          Type type;
          std::tie(name, type) = expression->convert(e, out);
          e.set_return_type(type);
          out->instructions.emplace_back(name, "return", name, name, type, type, type);
          return out;
        }
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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

        virtual BBlock* convert(Environment& e, BBlock *out)
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
        virtual void dump(std::ostream& stream=std::cout, int depth = 0) {
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
