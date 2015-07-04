// $Id: index_expression_node.h,v 1.4 2015/05/19 10:10:29 ist175714 Exp $
#ifndef __PWN_NODE_EXPRESSION_INDEX_H__
#define __PWN_NODE_EXPRESSION_INDEX_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>
#include <string>

namespace pwn {

  /**
   * Class for describing syntactic tree leaves for holding index expressions.
   */
  class index_expression_node: public cdk::expression_node {
  
  private:
    cdk::expression_node *_expression;
    cdk::expression_node *_shift;
    
  public:
    inline index_expression_node(int lineno, cdk::expression_node *node, cdk::expression_node *shift) :
        cdk::expression_node(lineno), _expression(node), _shift(shift) {
          type(new basic_type(8, basic_type::TYPE_DOUBLE));
    }

    inline cdk::expression_node *shift() {return _shift;}
    inline cdk::expression_node *expression() {return _expression;}

    /**
     * @param sp semantic processor visitor
     * @param level syntactic tree level
     */
    virtual void accept(basic_ast_visitor *sp, int level) {
      sp->do_index_expression_node(this, level);
    }

  };

} // pwn

#endif
