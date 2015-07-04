// $Id: postfix_writer.h,v 1.16 2015/05/20 09:40:14 ist175714 Exp $ -*- c++ -*-
#ifndef __PWN_SEMANTICS_PF_WRITER_H__
#define __PWN_SEMANTICS_PF_WRITER_H__

#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace pwn {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<pwn::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    int _current_offset, _current_function_end;
    std::string *_current_id;
    std::shared_ptr<symbol> _current_function;
    std::vector<std::string *> _declared_functions;
    std::vector<std::string *> _defined_functions;
    std::vector<std::string *> _called_functions;
    
    std::vector<int> _repeat_increment;
    std::vector<int> _repeat_end;
    
    std::stack<std::pair<std::string, std::string>> _cycles;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<pwn::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _current_offset(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }
    
    inline std::string fix_id(std::string id) {
      if (id == "pwn") return std::string("_main");
      if (id == "_main") return "._main";
      return id;
    }
    
  private:
    //!
    //! Methods to check types
    //!
    bool is_int(basic_type *type) {
      return type->name() == basic_type::TYPE_INT;
    }
    
    bool is_double(basic_type *type) {
      return type->name() == basic_type::TYPE_DOUBLE;
    }
    
    bool is_pointer(basic_type *type) {
      return type->name() == basic_type::TYPE_POINTER;
    }
    
    bool is_string(basic_type *type) {
      return type->name() == basic_type::TYPE_STRING;
    }
    
    bool is_void(basic_type *type) {
      return type->name() == basic_type::TYPE_VOID;
    }
    
    bool is_lval(cdk::basic_node * const node) {
      std::string name(node->name());
      return name == "index_node" || name == "lvalue_node"; 
    }

  public:
    void do_sequence_node(cdk::sequence_node * const node, int lvl);

  public:
    void do_integer_node(cdk::integer_node * const node, int lvl);
    void do_double_node(cdk::double_node * const node, int lvl);
    void do_string_node(cdk::string_node * const node, int lvl);

  public:
    void do_neg_node(cdk::neg_node * const node, int lvl);

  public:
    void do_add_node(cdk::add_node * const node, int lvl);
    void do_sub_node(cdk::sub_node * const node, int lvl);
    void do_mul_node(cdk::mul_node * const node, int lvl);
    void do_div_node(cdk::div_node * const node, int lvl);
    void do_mod_node(cdk::mod_node * const node, int lvl);
    void do_lt_node(cdk::lt_node * const node, int lvl);
    void do_le_node(cdk::le_node * const node, int lvl);
    void do_ge_node(cdk::ge_node * const node, int lvl);
    void do_gt_node(cdk::gt_node * const node, int lvl);
    void do_ne_node(cdk::ne_node * const node, int lvl);
    void do_eq_node(cdk::eq_node * const node, int lvl);

  public:
    void do_while_node(cdk::while_node * const node, int lvl);
    void do_if_node(cdk::if_node * const node, int lvl);
    void do_if_else_node(cdk::if_else_node * const node, int lvl);

  public:
    void do_simple_lvalue_node(pwn::simple_lvalue_node * const node, int lvl);
    void do_rvalue_node(pwn::rvalue_node * const node, int lvl);

  public:
    void do_evaluation_node(pwn::evaluation_node * const node, int lvl);
    void do_print_node(pwn::print_node * const node, int lvl);
    void do_read_node(pwn::read_node * const node, int lvl);
    void do_assignment_node(pwn::assignment_node * const node, int lvl);

    void do_and_node(pwn::and_node * const node, int lvl);
    void do_address_node(pwn::address_node * const node, int lvl);
    void do_function_call_node(pwn::function_call_node * const node, int lvl);
    void do_function_declaration_node(pwn::function_declaration_node * const node, int lvl);
    void do_function_definition_node(pwn::function_definition_node * const node, int lvl);
    void do_identity_node(pwn::identity_node * const node, int lvl);
    void do_next_node(pwn::next_node * const node, int lvl);
    void do_or_node(pwn::or_node * const node, int lvl);
    void do_repeat_node(pwn::repeat_node * const node, int lvl);
    void do_return_node(pwn::return_node * const node, int lvl);
    void do_stop_node(pwn::stop_node * const node, int lvl);
    void do_not_node(pwn::not_node * const node, int lvl);
    void do_index_node(pwn::index_node * const node, int lvl);
    void do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl);
    void do_memory_allocation_node(pwn::memory_allocation_node * const node, int lvl);
    void do_noob_node(pwn::noob_node * const node, int lvl);
    void do_block_node(pwn::block_node * const node, int lvl);
    void do_index_expression_node(pwn::index_expression_node * const node, int lvl);



  };

} // pwn

#endif
