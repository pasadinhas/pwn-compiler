#ifndef __PWN_FUNCTIONDEFINITIONNODE_H__
#define __PWN_FUNCTIONDEFINITIONNODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>
#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>


namespace pwn {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::basic_node {
    std::string _name;
    cdk::sequence_node *_arguments;
    basic_type *_return_type;
    block_node *_block;
    cdk::expression_node *_return_default;
    bool _is_local;

  public:
    inline function_definition_node(int lineno, std::string *name, cdk::sequence_node *arguments, basic_type *return_type, block_node *block, cdk::expression_node *_return_default = nullptr, bool is_local = false) :
        cdk::basic_node(lineno), _name(*name), _arguments(arguments), _return_type(return_type), _block(block), _return_default(_return_default), _is_local(is_local) {
    }

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
    inline block_node *block() {
      return _block;
    }
    inline cdk::expression_node *return_default() {
      return _return_default;
    }
    inline bool is_local() { return _is_local; }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // pwn

#endif
