#ifndef __PWN_NEXTNODE__
#define __PWN_NEXTNODE__

#include <cdk/ast/basic_node.h>

namespace pwn {

  /**
   * Class for describing next nodes.
   */
  class next_node: public cdk::basic_node {
    int _n;

  public:
    inline next_node(int lineno, int n = 1) :
        cdk::basic_node(lineno), _n(n){
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_next_node(this, level);
    }

    int value() {return _n;}

  };

} // pwn

#endif
