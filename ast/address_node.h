#ifndef __PWN_ADDRESSNODE__
#define __PWN_ADDRESSNODE__

#include <cdk/ast/unary_expression_node.h>
#include <cdk/ast/expression_node.h>

namespace pwn {

  /**
   * Class for describing the adress operator
   */
  class address_node: public cdk::unary_expression_node {
    
    //lvalue_node * _argument;
    
  public:
    inline address_node(int lineno, cdk::expression_node *arg) :
        cdk::unary_expression_node(lineno, arg) {
    }

    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }
    /*
    lvalue_node *argument() {
      return _argument;
    }*/

  };

} // pwn

#endif