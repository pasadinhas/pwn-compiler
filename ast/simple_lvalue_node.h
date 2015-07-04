// $Id: simple_lvalue_node.h,v 1.2 2015/05/19 10:31:26 ist175714 Exp $
#ifndef __PWN_NODE_EXPRESSION_LEFTVALUE_H__
#define __PWN_NODE_EXPRESSION_LEFTVALUE_H__

#include <cdk/ast/simple_value_node.h>
#include <string>

namespace pwn {

  /**
   * Class for describing syntactic tree leaves for holding lvalues.
   */
  class simple_lvalue_node: public lvalue_node {
    
    std::string *_value;
    
  public:
    inline simple_lvalue_node(int lineno, std::string *s) :
        lvalue_node(lineno), _value(s) {
    }
    
    std::string value() { return *_value; }

    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    virtual void accept(basic_ast_visitor *sp, int level) {
      sp->do_simple_lvalue_node(this, level);
    }

  };

} // pwn

#endif
