#ifndef __PWN_REPEATNODE_H__
#define __PWN_REPEATNODE_H__

#include <cdk/ast/sequence_node.h>

namespace pwn {

  /**
   * Class for describing repeat-cycle nodes.
   */
  class repeat_node: public cdk::basic_node {
    cdk::sequence_node *_inicializations;
    cdk::sequence_node *_conditions;
    cdk::sequence_node *_updates;
    cdk::basic_node *_block;

  public:
    inline repeat_node(int lineno, cdk::sequence_node *inicializations, cdk::sequence_node *conditions, cdk::sequence_node *updates, cdk::basic_node *block) :
        cdk::basic_node(lineno), _inicializations(inicializations), _conditions(conditions), _updates(updates), _block(block) {
    }

  public:
    inline cdk::sequence_node *inicializations() {
      return _inicializations;
    }
    inline cdk::sequence_node *conditions() {
      return _conditions;
    }
    inline cdk::sequence_node *updates() {
      return _updates;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_repeat_node(this, level);
    }

  };

} // pwn

#endif
