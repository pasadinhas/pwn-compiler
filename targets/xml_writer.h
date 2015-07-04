// $Id: xml_writer.h,v 1.10 2015/05/19 10:17:00 ist176012 Exp $ -*- c++ -*-
#ifndef __PWN_SEMANTICS_XMLWRITER_H__
#define __PWN_SEMANTICS_XMLWRITER_H__

#include <string>
#include <iostream>
#include <cdk/ast/basic_node.h>
#include <cdk/symbol_table.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

#define CONST_BIT 1UL<<8

namespace pwn {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class xml_writer: public basic_ast_visitor {
    cdk::symbol_table<pwn::symbol> &_symtab;

  public:
    xml_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<pwn::symbol> &symtab) :
        basic_ast_visitor(compiler), _symtab(symtab) {
    }

  public:
    ~xml_writer() {
      os().flush();
    }

  private:
    inline void openTag(const std::string &tag, int lvl) {
      os() << std::string(lvl, ' ') + "<" + tag + ">" << std::endl;
    }
    inline void openTag(const cdk::basic_node *node, int lvl) {
      openTag(node->name(), lvl);
    }
    inline void closeTag(const std::string &tag, int lvl) {
      os() << std::string(lvl, ' ') + "</" + tag + ">" << std::endl;
    }
    inline void closeTag(const cdk::basic_node *node, int lvl) {
      closeTag(node->name(), lvl);
    }
    
    inline void doCanBeNull(std::string name, cdk::basic_node *node, int lvl) {
      if (node) {
        openTag(name, lvl);
        node->accept(this, lvl + 2);
        closeTag(name, lvl);
      } else {
        os() << std::string(lvl, ' ') << "<" + name + "></" + name + ">" << std::endl;
      }
    }

  public:
    void do_sequence_node(cdk::sequence_node * const node, int lvl);

  protected:
    template<typename T>
    void processSimple(cdk::simple_value_node<T> * const node, int lvl) {
      os() << std::string(lvl, ' ') << "<" << node->name() << ">" << node->value() << "</" << node->name() << ">" << std::endl;
    }

    void processJustTag(cdk::basic_node * const node, int lvl) {
      openTag(node, lvl);
      closeTag(node, lvl);
    }
    
    void processReturnType(basic_type *type, int lvl) {
        os() << std::string(lvl, ' ') << "<returnType>" << typeCodeToString(type->name()) << "</returnType>" << std::endl;
    }

    void processVarType(basic_type *type, int lvl) {
        os() << std::string(lvl, ' ') << "<type>" << typeCodeToString(type->name()) << "</type>" << std::endl;
    }

    std::string typeCodeToString(unsigned long int u) {
        switch (u) {
          case (basic_type::TYPE_INT) : return "int";
          case (basic_type::TYPE_DOUBLE) : return "double";
          case (basic_type::TYPE_STRING) : return "string";
          case (basic_type::TYPE_VOID) : return "void";
          case (basic_type::TYPE_POINTER) : return "pointer";
          case (basic_type::TYPE_INT | CONST_BIT): return "const int";
          case (basic_type::TYPE_DOUBLE | CONST_BIT) : return "const double";
          case (basic_type::TYPE_STRING | CONST_BIT) : return "const string";
          case (basic_type::TYPE_POINTER | CONST_BIT) : return "const pointer";
          default : return "unknown type";
        }
    }


  public:
    void do_integer_node(cdk::integer_node * const node, int lvl);
    void do_string_node(cdk::string_node * const node, int lvl);
    void do_double_node(cdk::double_node * const node, int lvl);

  protected:
    void processUnaryExpression(cdk::unary_expression_node * const node, int lvl);

  public:
    void do_neg_node(cdk::neg_node * const node, int lvl);

  protected:
    void processBinaryExpression(cdk::binary_expression_node * const node, int lvl);

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

  public:
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
