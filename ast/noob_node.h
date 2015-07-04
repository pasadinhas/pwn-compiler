#ifndef __PWN_NOOBNODE__
#define __PWN_NOOBNODE__

#include <cdk/ast/expression_node.h>

namespace pwn {

  /**
   * Class for describing noob nodes.
   */
  class noob_node: public cdk::expression_node {

  public:
    inline noob_node(int lineno) :
        cdk::expression_node(lineno) {
          type(new basic_type(4, basic_type::TYPE_POINTER));
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_noob_node(this, level);
    }

  };

} // pwn

#endif
