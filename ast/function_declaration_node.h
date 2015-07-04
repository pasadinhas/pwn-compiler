#ifndef __PWN_FUNCTIONDECLARATIONNODE_H__
#define __PWN_FUNCTIONDECLARATIONNODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <cdk/ast/identifier_node.h>


namespace pwn {

  /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::basic_node {
    std::string _name;
    cdk::sequence_node *_arguments;
    basic_type *_return_type;
    bool _is_local, _is_import;

  public:
    inline function_declaration_node(
      int lineno, std::string *name, 
      cdk::sequence_node *arguments, 
      basic_type *return_type,
      bool is_local = false,
      bool is_import = false) :
        cdk::basic_node(lineno), 
        _name(*name), _arguments(arguments), 
        _return_type(return_type),
        _is_local(is_local),
        _is_import(is_import) {}

  public:
    inline std::string name() {
      return _name;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    inline basic_type *return_type() {
      return _return_type;
    }
    inline bool is_local() { return _is_local; }
    inline bool is_import() { return _is_import; }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // pwn

#endif
