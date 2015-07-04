// $Id: memory_allocation_node.h,v 1.2 2015/05/19 11:34:58 ist176012 Exp $
#ifndef __PWN_MEMORY_ALLOCATION_NODE_H__
#define __PWN_MEMORY_ALLOCATION_NODE_H__

#include <cdk/ast/expression_node.h>

namespace pwn {

  /**
   * Class for describing memory allocation nodes.
   */
  class memory_allocation_node: public cdk::expression_node {
    cdk::expression_node* _size;

  public:
    inline memory_allocation_node(int lineno, cdk::expression_node* size) :
        cdk::expression_node(lineno), _size(size) {
    }

  public:
    inline cdk::expression_node* size() {
      return _size;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_memory_allocation_node(this, level);
    }

  };

} // pwn

#endif
