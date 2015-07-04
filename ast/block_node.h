#ifndef __PWN_BLOCKNODE_H__
#define __PWN_BLOCKNODE_H__

#include <cdk/ast/sequence_node.h>

namespace pwn {

  /**
   * Class for describing block nodes.
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_declarations;
    cdk::sequence_node *_statements;

  public:
    inline block_node(int lineno, cdk::sequence_node *declarations = nullptr, cdk::sequence_node *statements = nullptr) :
        cdk::basic_node(lineno), _declarations(declarations), _statements(statements) {
    }

  public:
    inline cdk::sequence_node *declarations() {
      return _declarations;
    }
    inline cdk::sequence_node *statements() {
      return _statements;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // pwn

#endif
