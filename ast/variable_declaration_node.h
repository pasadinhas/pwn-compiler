#ifndef __PWN_VARIABLEDECLARATIONNODE_H__
#define __PWN_VARIABLEDECLARATIONNODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <cdk/ast/identifier_node.h>


namespace pwn {

  /**
   * Class for describing function declaration nodes.
   */
  class variable_declaration_node: public cdk::basic_node {
    std::string _name;
    basic_type *_type;
    cdk::expression_node *_inicial_value;
    bool _is_local;
    bool _is_import;

  public:
    inline variable_declaration_node(
      int lineno,
      std::string *name, 
      basic_type *type,
      cdk::expression_node *inicial_value = nullptr, 
      bool is_local = false,
      bool is_import = false ) :
        cdk::basic_node(lineno), 
        _name(*name), 
        _type(type),
        _inicial_value(inicial_value),
        _is_local(is_local),
        _is_import(is_import) {
    }

  public:
    inline std::string name() {
      return _name;
    }
    inline basic_type *type() {
      return _type;
    }
    inline cdk::expression_node *initial_value() {
      return _inicial_value;
    }
    inline bool is_local() {
      return _is_local;
    }
    inline bool is_import() {
      return _is_import;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // pwn

#endif
