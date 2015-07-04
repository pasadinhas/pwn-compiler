#ifndef __PWN_FUNCTIONCALLNODE_H__
#define __PWN_FUNCTIONCALLNODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <cdk/ast/identifier_node.h>
#include <cdk/ast/expression_node.h>


namespace pwn {

  /**
   * Class for describing function declaration nodes.
   */
  class function_call_node: public cdk::expression_node {
    std::string _name;
    cdk::sequence_node *_arguments;

  public:
    inline function_call_node(int lineno, std::string *name, cdk::sequence_node *arguments) :
        cdk::expression_node(lineno), _name(*name), _arguments(arguments) {
    }

  public:
    inline std::string name() {
      return _name;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // pwn

#endif
