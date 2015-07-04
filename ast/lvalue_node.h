// $Id: lvalue_node.h,v 1.3 2015/05/19 11:34:58 ist176012 Exp $
#ifndef __PWN_NODE__LEFTVALUE_H__
#define __PWN_NODE__LEFTVALUE_H__

#include <cdk/ast/expression_node.h>
#include <string>

namespace pwn {

  /**
   * Class for describing syntactic tree leaves for holding lvalues.
   */
  class lvalue_node: public cdk::expression_node {
    
  public:
    inline lvalue_node(int lineno) :
        expression_node(lineno) {
    }

    virtual void accept(basic_ast_visitor *sp, int level) = 0;

  };

} // pwn

#endif
