#ifndef __PWN_STOPNODE__
#define __PWN_STOPNODE__

#include <cdk/ast/basic_node.h>

namespace pwn {

  /**
   * Class for describing stop nodes.
   */
  class stop_node: public cdk::basic_node {

  int _n;

  public:
    inline stop_node(int lineno, int n = 1) :
        cdk::basic_node(lineno), _n(n){
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stop_node(this, level);
    }

    int value() {return _n;}
  };

} // pwn

#endif
