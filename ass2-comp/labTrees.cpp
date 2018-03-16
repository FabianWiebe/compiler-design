#include "labTrees.hh"

std::string get_print_parm(Type type) {
    switch(type) {
      case Type::LONG: {
        return "%ld";
      }
      case Type::DOUBLE: {
        return "%lg"; //f
      }
      case Type::BOOL: {
        return "%d";
      }
      case Type::STRING: {
        return "%s";
      }
    }
    throw std::invalid_argument( std::string("Cannot print type ") + type_as_string(type) );
}

std::string get_jmp_code(const std::string& comp) {
  if (comp == "==") {
    return "je";
  } else if (comp == "!=") {
    return "jne";
  } else if (comp == "<") {
    return "jl";
  } else if (comp == ">") {
    return "ja";
  } else if (comp == "<=") {
    return "jbe";
  } else if (comp == ">=") {
    return "jae";
  }
  throw std::invalid_argument( std::string("Invalid comparison ") + comp );
}
std::string combine(const std::list<std::string>& list, const std::string& delimeter) {
  if (list.empty()) return "";
  auto itr = list.begin();
  std::string str = *itr;
  for(++itr; itr != list.end(); ++itr) {
    str += delimeter + *itr;
  }
  return str;
}

std::string create_format_string(const std::list<Type>& types, const std::string& function) {
  if (function == "io.read") return "%ld";
  std::list<std::string> formatted_types;
  for (auto type : types) {
    formatted_types.push_back(get_print_parm(type));
  }
  std::string delimeter = "", end_line = "";
  if (function == "print") {
    delimeter = "\\t";
    end_line = "\\n";
  }
  return combine(formatted_types, delimeter) + end_line;
}


std::pair<std::string, Type> Command::convert(Environment& e, BBlock *out, bool is_expression) {
  std::list<std::string> values;
  std::list<Type> types;
  for (Expression* par : parameters) {
    std::string value;
    Type type;
    std::tie(value, type) = par->convert(e, out);
    values.push_back(value);
    types.push_back(type);
  }
  std::string return_name = "", first = "", second = "";
  Type first_type = Type::UNDEFINED, second_type = Type::UNDEFINED, return_type = Type::VOID;
  auto itr = values.begin();
  auto t_itr = types.begin();
  if (itr != values.end()) {
    first = *itr;
    first_type = *t_itr;
    if (++itr != values.end()) {
      second = *itr;
      second_type = *++t_itr;
    }
  }
  if (name != "io.read" && name != "io.write" && name != "print") {
    return_type = e.get_function(name)->dump_function(e, types);
  } else if (name == "io.read") {
    name = "scanf";
    first = e.makeNames();
    e.store(first, Value(std::string("%ld")));
    e.read_used = true;
    return_type = Type::LONG;
  } else { // io.write, print
    if (is_expression) {
      throw std::invalid_argument( std::string("Void function ") + name +  " has no return argument." );
    }
    if (!values.empty()) {
      for (Type type : types) if (type == Type::DOUBLE || type == Type::LONG) e.convert_num_to_string = true;
      auto values_itr = values.begin();
      auto types_itr = types.begin();
      std::list<std::string> tmp_values{*values_itr};
      std::list<Type> tmp_types{*types_itr};
      out->instructions.emplace_back(return_name, "call", "printf", *values_itr, Type::STRING, *types_itr, return_type, tmp_values, tmp_types);
      for (++values_itr, ++types_itr; values_itr != values.end(); ++values_itr, ++types_itr) {
        if (name == "print") {
          std::list<std::string> tmp_values{e.get_tab_str()};
          std::list<Type> tmp_types{Type::STRING};
          out->instructions.emplace_back(return_name, "call", "printf", e.get_tab_str(), Type::STRING, Type::STRING, return_type, tmp_values, tmp_types);
        }
        std::list<std::string> tmp_values{*values_itr};
        std::list<Type> tmp_types{*types_itr};
        out->instructions.emplace_back(return_name, "call", "printf", *values_itr, Type::STRING, *types_itr, return_type, tmp_values, tmp_types);
      }
    }
    if (name == "print") { // print new line at end
      std::list<std::string> tmp_values{e.get_newline_str()};
      std::list<Type> tmp_types{Type::STRING};
      out->instructions.emplace_back(return_name, "call", "printf", e.get_newline_str(), Type::STRING, Type::STRING, return_type, tmp_values, tmp_types);
    }
    return {return_name, return_type};
  }
  if (is_expression) {
    if (return_type == Type::VOID) {
      throw std::invalid_argument( std::string("Void function ") + name +  " has no return argument." );
    }
    // use return value
    return_name = e.makeNames(return_type);
  }
  std::list<std::pair<std::string, Type>> parms_for_stack;
  if (e.recursive_call(name)) {
    parms_for_stack = e.get_function_vars(name);
  }
  out->instructions.emplace_back(return_name, "call", name, first, first_type, second_type, return_type, values, types, parms_for_stack);
  return {return_name, return_type};
}

void output_start_of_asm(std::ostream& stream) {
  stream << "  asm(" << std::endl;
}

std::string format_type(Type type) {
  if (type == Type::DOUBLE) {
    return "x";
  }
  return "g";
}

void output_end_of_asm(std::ostream& stream, const std::list<std::pair<std::string, Type>>& var_names) {
  stream << ":";
  if (!var_names.empty()) {
    auto itr = var_names.begin();
    stream << " [" << itr->first << "] \"+" << format_type(itr->second) << "\" (" << itr->first << ")";
    for (++itr; itr != var_names.end(); ++itr) {
      stream << "," << std::endl << "  [" << itr->first << "] \"+" << format_type(itr->second) << "\" (" << itr->first << ")";
    }
  }
  stream << R"(
:
: "rax", "rbx", "rdx", "cc", "xmm0", "xmm1", "rdi", "rsi", "rsp", "rcx"
  );
)";
}


std::list<Type> types{Type::LONG, Type::DOUBLE, Type::BOOL, Type::STRING};

std::string type_to_string(Type type, std::string name) {
  if (type == Type::ARRAY) {
      return std::string("double ") + name + "[]";
    } else if (type == Type::STRING) {
      return std::string("char ") + name + "[]";
    } else {
      return type_as_string(type) + " " + name;
    }
}

std::string escape_new_lines(const std::string& str) {
  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find("\\n", pos)) != std::string::npos) {
      result.insert(pos, "\\");
      pos += 3;
  }
  return result;
}

void define_vars(std::ostream& stream, Environment& e, bool esc_str) {
  for (Type type : types) {
    auto var_names = e.get_all_of_type(type);
    if (!var_names.empty()) {
      auto itr = var_names.begin();
      stream << "" << type_as_string(type) << " " << *itr;
      for (++itr; itr != var_names.end(); ++itr) {
        stream << ", " << *itr;
      }
      stream << ";" << std::endl;
    }
  }
  for (auto& pair : e.get_const_values()) {
    Type type = pair.second.type;
    stream << type_to_string(type, pair.first) << " = ";
    if (type == Type::STRING) {
      if (esc_str) {
        stream << "\\\"" << escape_new_lines(pair.second.as_string()) << "\\\";" << std::endl;;
      } else {
        stream << "\"" << pair.second << "\";" << std::endl;;
      }
    } else if (type == Type::ARRAY) {
      stream << "{" << pair.second.as_array().size() << ", " << pair.second << "}; // array size at "<< pair.first << "[0]" << std::endl;
    } else {
      stream << pair.second << ";" << std::endl;
    }
  }
}

void define_function_vars_asm(std::ostream& stream, const std::list<std::string>& names, const std::list<Type> types) {
  auto name_itr = names.begin();
  for (auto types_itr = types.begin(); types_itr != types.end(); ++types_itr, ++name_itr) {
      stream << *name_itr<< ":\t.";
      if (*types_itr == Type::LONG) {
        stream << "quad 0";
      } else if (*types_itr == Type::DOUBLE){
        stream << type_as_string(*types_itr) << " 0";
      }
      stream << std::endl;    
  }
}

void define_vars_asm(std::ostream& stream, Environment& e) {
  for (Type type : types) {
    for (auto& var : e.get_all_of_type(type)) {
      stream << var << ":\t.";
      if (type == Type::LONG) {
        stream << "quad 0";
      } else if (type == Type::DOUBLE){
        stream << type_as_string(type) << " 0";
      }
      stream << std::endl;
    }
  }
  for (auto& pair : e.get_const_values()) {
    Type type = pair.second.type;
    stream << pair.first << ":\t.";
    if (type == Type::STRING) {
      stream << "string \"" << pair.second << "\"" << std::endl;
      stream << pair.first << "_length:\t.quad " << pair.second.as_string().size();
    } else if (type == Type::ARRAY) {
      auto& array = pair.second.as_array();
      auto itr = array.begin();
      stream << "quad " << array.size() << " # array size is placed at position " << pair.first << "[0]";
      for (auto val : array) {
        stream << std::endl << "\t\t.double " << val;
      }
    } else if (type == Type::DOUBLE){
      stream << "double " << pair.second;
    }
    stream << std::endl;
  }
  if (e.convert_num_to_string) {
    stream << "\t\t.lcomm _fpconv_buf, 32 # used for double/long to string conversion in fpconv" << std::endl;
  }
  // will be delted when libc is removed:
  stream << "_print_string:\t.string \"%s\"" << std::endl;
  stream << "_print_double:\t.string \"%lg\"" << std::endl;
  stream << "_print_long:\t.string \"%ld\"" << std::endl;
}

void output_vars(std::ostream& stream, Environment& e) {
  for (Type type : types) {
    auto var_names = e.get_all_of_type(type);
    for (const std::string& var_name : var_names) {
      stream << "  printf(\"" << var_name << " = " << get_print_parm(type) << "\\n\", " << var_name << ");" << std::endl;
    }
  }
}


void output_vars(std::ostream& stream, std::list<std::pair<std::string, Type>>& vars) {
  for (auto & pair : vars) {
    stream << "  printf(\"" << pair.first << " = " << get_print_parm(pair.second) << "\\n\", " << pair.first << ");" << std::endl;
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

void dump_blocks(BBlock *start, std::ostream& stream, void (BBlock::*func)(std::ostream&)) {
  std::set<BBlock *> done, todo;
  todo.insert(start);
  while(todo.size()>0)
  {
          // Pop an arbitrary element from todo set
          auto first = todo.begin();
          BBlock *next = *first;
          todo.erase(first);
          (next->*func)(stream);
          stream << std::endl;
          done.insert(next);
          if(next->tExit!=NULL && done.find(next->tExit)==done.end())
                  todo.insert(next->tExit);
          if(next->fExit!=NULL && done.find(next->fExit)==done.end())
                  todo.insert(next->fExit);
  }
}

void get_parms_from_reg(std::ostream& stream, const std::list<std::string>& names, const std::list<Type>& types) {
  std::list<std::string> std_regs{"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  std::list<std::string> dbl_regs{"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"};
  auto std_reg_itr = std_regs.begin();
  auto dbl_reg_itr = dbl_regs.begin();
  auto types_itr = types.begin();
  size_t counter_dbl = 0, counter_std = 0;
  for (auto itr = names.begin(); itr != names.end(); ++itr, ++types_itr) {
    if (*types_itr == Type::DOUBLE) {
      if (dbl_reg_itr == dbl_regs.end()) throw std::invalid_argument( "Can only pass 8 doubles as parameters." );
      stream << "\t\tmovsd %" << *dbl_reg_itr++ << ", " << *itr << " # double Arg " << ++counter_dbl << std::endl;
    } else if (*types_itr == Type::STRING) {
      if (std_reg_itr == std_regs.end()) throw std::invalid_argument( "Can only pass 6 standard parameters. Pushing parameters on stack not implemented." );
      stream << "\t\tmovq %" << *std_reg_itr++ << ", " << *itr << " # Arg " << ++counter_std << std::endl;
    } else {
      if (std_reg_itr == std_regs.end()) throw std::invalid_argument( "Can only pass 6 standard parameters. Pushing parameters on stack not implemented." );
      stream << "\t\tmovq %" << *std_reg_itr++ << ", " << *itr << " # Arg " << ++counter_std << std::endl;
    }
  }
}

/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpASM(Environment& e, BBlock *start, std::ostream& stream)
{
        stream << ".data" << std::endl;

        define_vars_asm(stream, e);
        for (auto& pair : e.get_functions()) {
          Function* func = pair.second;
          define_function_vars_asm(stream, func->parameter_names, func->parameter_types);
        }

        stream << ".text" << std::endl << ".globl _start" << std::endl;
        if (e.pow_used) {
          stream << "pow: # pow function begin" << std::endl;
          stream << "\t\tmovq %rdi, %rax" << std::endl;
          stream << "pow_loop:" << std::endl;
          stream << "\t\tcmp $1, %rsi" << std::endl;
          stream << "\t\tjbe pow_ret" << std::endl;
          stream << "\t\tdec %rsi" << std::endl;
          stream << "\t\tmul %rdi" << std::endl;
          stream << "\t\tjmp pow_loop" << std::endl;
          stream << "pow_ret:" << std::endl;
          stream << "\t\tret # pow function end" << std::endl;
        }
        if (e.convert_num_to_string) {
          stream << R"(
fpconv:
    movq %rax, %r11
    movq %rdi, %rax
    lea _fpconv_buf, %rdi
    xorq %rcx, %rcx # index = 0
    movq $10, %rsi
    testq %r11, %r11 # check if long or double
    jz .fpconv_first_loop
    cvttsd2siq %xmm0, %rax # paramter in xmm0
.fpconv_first_loop:
    testq %rax, %rax # rax == 0
    jz .fpconv_done
    xorq %rdx, %rdx # zero out high part of div
    divq %rsi # / 10
    addb $48, %dl # + '0'
    movb %dl, (%rdi, %rcx) # push to buf
    incq %rcx # ++index
    jmp .fpconv_first_loop
.fpconv_done:
    movq %rcx, %rsi
    sarq %rsi
    decq %rcx # --index
    xorq %r8, %r8 # r8 = j
.fpconv_revers:
    testq %rsi, %rsi
    je .fpconv_end_revers
    movq %rcx, %r9 # copy index
    subq %r8, %r9 # r9 = index - j
    movb (%rdi, %r8), %al
    movb (%rdi, %r9), %dl
    movb %al, (%rdi, %r9)
    movb %dl, (%rdi, %r8)
    incq %r8 # ++j
    decq %rsi
    jmp .fpconv_revers
.fpconv_end_revers:
    incq %rcx # ++indx
    testq %r11, %r11 # check if long or double
    je .fpconv_ret # skip decimals, if long
    movb $46, (%rdi, %rcx) # dot
    incq %rcx
    movq $1, %rsi
    movq $10, %r8
    movq $6, %r9 # 5 decimal places
.fpconv_dec_loop:
    decq %r9
    jz .fpconv_ret
    movq %rsi, %rax
    mulq %r8
    movq %rax, %rsi
    cvtsi2sdq %rsi, %xmm1
    mulsd %xmm0, %xmm1
    cvttsd2siq %xmm1, %rax
    xorq %rdx, %rdx
    divq %r8
    addb $48, %dl
    movb %dl, (%rdi, %rcx) # push to buf
    incq %rcx # ++index
    jmp .fpconv_dec_loop
.fpconv_ret:
    # movb $0, (%rdi, %rcx) # null value
    # incq %rcx # ++index
    # cmpq $32, %rcx
    # jl .fpconv_ret
    movq %rdi, %rax # return value
    movq %rcx, %rdx # 2nd return val, number of bytes
    ret
)";
        }

        for (auto & pair : e.get_functions()) {
          Function* func = pair.second;
          if (func->first_block) {
            stream << func->name << ": # " << func->name << " function begin" << std::endl;
            get_parms_from_reg(stream, func->parameter_names, func->parameter_types);
            dump_blocks(func->first_block, stream, &BBlock::dump);
            stream << "\t\tret # " << func->name << " function end" << std::endl;
          }
        }
        stream << "_start: # main begin" << std::endl;

        dump_blocks(start, stream, &BBlock::dump);

        stream << "\t\t# exit(0)" << std::endl;
        stream << "\t\tmovq $60, %rax # syscal call for exit" << std::endl;
        stream << "\t\txorq $0, %rdi # return code" << std::endl;
        stream << "\t\tsyscall" << std::endl;
        stream << "\t\t# main end" << std::endl;
}

/*
 * Iterate over each basic block that can be reached from the entry point
 * exactly once, so that we can dump out the entire graph.
 * This is a concrete example of the graph-walk described in lecture 7.
 */
void dumpCFG(Environment& e, BBlock *start, std::ostream& stream)
{
        stream << "digraph {" << std::endl;

        stream << R"(declaration_block [shape=box, label="#include \"stdio.h\")" << std::endl;
        if (e.pow_used) stream << R"(#include \"math.h\")" << std::endl;
        if (e.read_used) stream << R"(#include \"stdlib.h\")" << std::endl;
        define_vars(stream, e, true);
        stream << "\"];" << std::endl;
        stream << "declaration_block -> " << start->name << ";" << std::endl;

        dump_blocks(start, stream, &BBlock::dumpCFG);

        for (auto & pair : e.get_functions()) {
          Function* func = pair.second;
          if (func->first_block) {
            stream << func->name << "_block [label=\"";
            stream << type_as_string(func->return_type) << " " << func->name << "(";
            if (!func->parameter_types.empty()) {
              auto type_itr = func->parameter_types.begin();
              auto name_itr = func->parameter_names.begin();
              stream << type_to_string(*type_itr, *name_itr);
              for(++type_itr, ++name_itr; type_itr != func->parameter_types.end(); ++type_itr, ++name_itr) {
                stream << ", " << type_to_string(*type_itr, *name_itr);
              }
              stream << ")\"]" << std::endl;
              stream << func->name << "_block -> " << func->first_block->name << ";" << std::endl;
              dump_blocks(func->first_block, stream, &BBlock::dumpCFG);
            }
          }
        }

        stream << "}" << std::endl;
}
