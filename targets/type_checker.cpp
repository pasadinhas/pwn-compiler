// $Id: type_checker.cpp,v 1.21 2015/05/19 20:29:57 ist176012 Exp $ -*- c++ -*-
#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

#define CONST_BIT 1UL<<8
//--------------------------------------------------------------------------

void pwn::type_checker::visitChildren(cdk::binary_expression_node * const node, int lvl) {
  //already validated
  if(node->type() != NULL) {
    return;
  }
  node->left()->accept(this, lvl+2);
  node->right()->accept(this, lvl+2);

}

//---------------------------------------------------------------------------

void pwn::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void pwn::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

void pwn::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

void pwn::type_checker::do_noob_node(pwn  ::noob_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
}

//---------------------------------------------------------------------------

inline void pwn::type_checker::processUnaryMathExpression(cdk::unary_expression_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
  } else if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE) {
     node->type(new basic_type(8, basic_type::TYPE_DOUBLE)); 
  } else {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in argument of unary expression");
  }

}

inline void pwn::type_checker::processUnaryLogicalExpression(cdk::unary_expression_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->type()->name() != basic_type::TYPE_INT) {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in argument of unary expression");
  } 

  node->type(new basic_type(4, basic_type::TYPE_INT));

}

void pwn::type_checker::do_not_node(pwn::not_node * const node, int lvl) {
  processUnaryLogicalExpression(node, lvl);
}

void pwn::type_checker::do_identity_node(pwn::identity_node * const node, int lvl) {
  processUnaryMathExpression(node, lvl);
}

void pwn::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryMathExpression(node, lvl);
}

//---------------------------------------------------------------------------

inline void pwn::type_checker::processBinaryMultiplicativeExpression(cdk::binary_expression_node * const node, int lvl) {
  visitChildren(node, lvl);

  if((node->left()->type()->name() == basic_type::TYPE_STRING)
  || (node->right()->type()->name() == basic_type::TYPE_STRING)
  || (node->left()->type()->name() == basic_type::TYPE_VOID)
  || (node->right()->type()->name() == basic_type::TYPE_VOID)
  || (node->left()->type()->name() == basic_type::TYPE_ERROR)
  || (node->right()->type()->name() == basic_type::TYPE_ERROR)
  || (node->left()->type()->name() == basic_type::TYPE_POINTER)
  || (node->right()->type()->name() == basic_type::TYPE_POINTER)) {

    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in arguments of binary arithmetic expression");
  }

  if((node->left()->type()->name() == basic_type::TYPE_INT)
  || (node->right()->type()->name() == basic_type::TYPE_POINTER)) {

    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  }

  if((node->left()->type()->name() == basic_type::TYPE_DOUBLE)
  || (node->right()->type()->name() == basic_type::TYPE_DOUBLE)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  }

  else {
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }
}

//----------------------------------------------------------------------------

inline void pwn::type_checker::processBinaryLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
visitChildren(node, lvl);

  if((node->left()->type()->name() != basic_type::TYPE_INT)
  || (node->right()->type()->name() != basic_type::TYPE_INT)) {

     node->type(new basic_type(0, basic_type::TYPE_ERROR));
     throw std::string("wrong type in right argument of binary logical expression");
  }

  node->type(new basic_type(4, basic_type::TYPE_INT));
}
//-----------------------------------------------------------------------------

inline void pwn::type_checker::processBinaryEqualityExpression(cdk::binary_expression_node * const node, int lvl) {
visitChildren(node, lvl);

  if((node->left()->type()->name() == basic_type::TYPE_STRING)
  || (node->right()->type()->name() == basic_type::TYPE_STRING)
  || (node->left()->type()->name() == basic_type::TYPE_VOID)
  || (node->right()->type()->name() == basic_type::TYPE_VOID)
  || (node->left()->type()->name() == basic_type::TYPE_ERROR)
  || (node->right()->type()->name() == basic_type::TYPE_ERROR)) {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in argument of binary equality logical expression");
  }

  node->type(new basic_type(4, basic_type::TYPE_INT)); //as in C
}

//-----------------------------------------------------------------------------------

inline void pwn::type_checker::processBinaryCompareExpression(cdk::binary_expression_node * const node, int lvl) {
  visitChildren(node, lvl);

  if((node->left()->type()->name() == basic_type::TYPE_VOID)
  || (node->right()->type()->name() == basic_type::TYPE_VOID)
  || (node->left()->type()->name() == basic_type::TYPE_ERROR)
  || (node->right()->type()->name() == basic_type::TYPE_ERROR)) {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in argument of binary compare expression");
  }

  node->type(new basic_type(4, basic_type::TYPE_INT)); //as in C
}

inline void pwn::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in right argument of binary expression");

  // in Simple, expressions are always int
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void pwn::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
  visitChildren(node, lvl);

  if((node->left()->type()->name() == basic_type::TYPE_STRING)
  || (node->right()->type()->name() == basic_type::TYPE_STRING)
  || (node->left()->type()->name() == basic_type::TYPE_VOID)
  || (node->right()->type()->name() == basic_type::TYPE_VOID)
  || (node->left()->type()->name() == basic_type::TYPE_ERROR)
  || (node->right()->type()->name() == basic_type::TYPE_ERROR)) {

    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in arguments of binary arithmetic expression");
  }

  else if((node->left()->type()->name() == basic_type::TYPE_INT)
  && (node->right()->type()->name() == basic_type::TYPE_POINTER)) {

    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_POINTER)
  && (node->right()->type()->name() == basic_type::TYPE_INT)) {

    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_DOUBLE)
  || (node->right()->type()->name() == basic_type::TYPE_DOUBLE)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_INT)
  || (node->right()->type()->name() == basic_type::TYPE_INT)) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }

  else {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in arguments of binary arithmetic expression");
  }
}
void pwn::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  visitChildren(node, lvl);

  if((node->left()->type()->name() == basic_type::TYPE_STRING)
  || (node->right()->type()->name() == basic_type::TYPE_STRING)
  || (node->left()->type()->name() == basic_type::TYPE_VOID)
  || (node->right()->type()->name() == basic_type::TYPE_VOID)
  || (node->left()->type()->name() == basic_type::TYPE_ERROR)
  || (node->right()->type()->name() == basic_type::TYPE_ERROR)) {

    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in arguments of binary arithmetic expression");
  }

  else if((node->left()->type()->name() == basic_type::TYPE_INT)
  && (node->right()->type()->name() == basic_type::TYPE_POINTER)) {

    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_POINTER)
  && (node->right()->type()->name() == basic_type::TYPE_INT)) {

    node->type(new basic_type(4, basic_type::TYPE_POINTER));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_DOUBLE)
  || (node->right()->type()->name() == basic_type::TYPE_DOUBLE)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_INT)
  || (node->right()->type()->name() == basic_type::TYPE_INT)) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }

  else if((node->left()->type()->name() == basic_type::TYPE_POINTER)
  || (node->right()->type()->name() == basic_type::TYPE_POINTER)) {
    node->type(new basic_type(4, basic_type::TYPE_INT));
  }
  else {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw std::string("wrong type in arguments of binary arithmetic expression");
  }
}         
void pwn::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  processBinaryMultiplicativeExpression(node, lvl);
}
void pwn::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  processBinaryMultiplicativeExpression(node, lvl);
}

void pwn::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {

  visitChildren(node, lvl);

  if((node->left()->type()->name() != basic_type::TYPE_INT)
  || (node->right()->type()->name() != basic_type::TYPE_INT)){  
    throw std::string("wrong type in argument of binary mod expression");
  }

  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void pwn::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryCompareExpression(node, lvl);
}
void pwn::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryCompareExpression(node, lvl);
}
void pwn::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryCompareExpression(node, lvl);
}
void pwn::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryCompareExpression(node, lvl);
}
void pwn::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryEqualityExpression(node, lvl);
}
void pwn::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryEqualityExpression(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::type_checker::do_rvalue_node(pwn::rvalue_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

//---------------------------------------------------------------------------
  
void pwn::type_checker::do_simple_lvalue_node(pwn::simple_lvalue_node * const node, int lvl) {
  const std::string &id = node->value();
  std::shared_ptr<pwn::symbol> symbol = _symtab.find(id);
  if (symbol == nullptr) {
        throw "type_checker::do_lvalue_node : " + id + " undeclared";
  } else {
        node->type(symbol->type());
  }
}

//---------------------------------------------------------------------------

void pwn::type_checker::do_assignment_node(pwn::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;
  
  node->lvalue()->accept(this, lvl + 2);
  node->rvalue()->accept(this, lvl + 2);

  // Check for invalid types in assignment
  if((node->lvalue()->type()->name() == basic_type::TYPE_VOID)
  || (node->rvalue()->type()->name() == basic_type::TYPE_VOID)
  || (node->lvalue()->type()->name() == (basic_type::TYPE_VOID | CONST_BIT))
  || (node->rvalue()->type()->name() == (basic_type::TYPE_VOID | CONST_BIT))
  || (node->lvalue()->type()->name() == basic_type::TYPE_ERROR)
  || (node->rvalue()->type()->name() == basic_type::TYPE_ERROR)
  || (node->lvalue()->type()->name() == basic_type::TYPE_UNSPEC)) {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw "Wrong type in assignment";
    return;
  }
  
  //if it a read a man must give types
  if(node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC  && (node->lvalue()->type()->name()== basic_type::TYPE_INT))
    node->rvalue()->type(new basic_type(4, basic_type::TYPE_INT));
  else if(node->rvalue()->type()->name() == basic_type::TYPE_UNSPEC  && (node->lvalue()->type()->name() == basic_type::TYPE_DOUBLE))
    node->rvalue()->type(new basic_type(8, basic_type::TYPE_DOUBLE));

  // We can assign the integer literal zero to a pointer
  else if((node->lvalue()->type()->name() == basic_type::TYPE_POINTER)
       && (node->rvalue()->name() == "noob_node")) {
    node->type(node->lvalue()->type()); // not necessary but...
  }
  // Otherwise, if operand types do not match, its an error
  else if(node->lvalue()->type()->name() != node->rvalue()->type()->name()) {
    node->type(new basic_type(0, basic_type::TYPE_ERROR));
    throw "Wrong type in assignment";
  } else {
    node->type(node->lvalue()->type());
  }

}

//---------------------------------------------------------------------------

void pwn::type_checker::do_evaluation_node(pwn::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void pwn::type_checker::do_print_node(pwn::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);

  if (node->argument()->type()->name() == basic_type::TYPE_POINTER)
   throw "It is not possible to print pointers";
  
  //If it is a read it is an integer
  if(node->argument()->type()->name() == basic_type::TYPE_UNSPEC)
    node->argument()->type(new basic_type(4, basic_type::TYPE_INT));
}

//---------------------------------------------------------------------------

void pwn::type_checker::do_read_node(pwn::read_node * const node, int lvl) {
  ASSERT_UNSPEC;
  
  node->type(new basic_type(4, basic_type::TYPE_UNSPEC)); // a man does not know the type of the read input
}

//---------------------------------------------------------------------------

void pwn::type_checker::do_while_node(cdk::while_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void pwn::type_checker::do_if_node(cdk::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void pwn::type_checker::do_if_else_node(cdk::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//----------------------------------------------------------------------------

void pwn::type_checker::do_and_node(pwn::and_node * const node, int lvl) {
    processBinaryLogicalExpression(node, lvl);
}

void pwn::type_checker::do_or_node(pwn::or_node * const node, int lvl) {
    processBinaryLogicalExpression(node, lvl);
}

void pwn::type_checker::do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl) {
  
  const std::string &id = node->name();
  std::shared_ptr<pwn::symbol> symbol = _symtab.find(id);
  if (!symbol) {
        _symtab.insert(id, std::make_shared<pwn::symbol>(node->type(), id, -1));
  } else {
        throw id + "redeclared";
  }

  if (node->initial_value() != nullptr) {
      node->initial_value()->accept(this, lvl +2);
      if (node->initial_value()->type()->name() != node->type()->name()) {
          throw "Expression is not of the variable type";        
      }
  }

}

void pwn::type_checker::do_function_declaration_node(pwn::function_declaration_node * const node, int lvl) {
  std::vector<basic_type*> args;

  if(node->arguments()!=NULL)
	  for (size_t i = 0; i < node->arguments()->size(); i++) {
	    pwn::variable_declaration_node *arg = (pwn::variable_declaration_node*) node->arguments()->node(i);
	    args.push_back(arg->type());
  }
  
  const std::string &id = node->name();
  if (!_symtab.find_local(id)) {
  	std::shared_ptr<pwn::symbol> s = std::make_shared<pwn::symbol>(node->return_type(), id, -1);
    s->is_var(false);
    s->args(args);
    s->offset(-1);
    _symtab.insert(id, s);
  }

  else
    throw "Redeclaration of: " + id;
}

void pwn::type_checker::do_function_definition_node(pwn::function_definition_node * const node, int lvl) {

    std::string id;
    std::vector<basic_type*> args;

    if(node->arguments()!=NULL) {
	    for (size_t i = 0; i < node->arguments()->size(); i++) {
	      pwn::variable_declaration_node *arg = (pwn::variable_declaration_node*) node->arguments()->node(i);
	      args.push_back(arg->type());
      }
    }

    if(node->return_default()) {
    	node->return_default()->accept(this, lvl);
    	if(node->return_default()->type()->name() != node->return_type()->name()) {
    	  throw std::string("default return value type and function type do not match");
      }
    }
    
    std::shared_ptr<pwn::symbol> symbol = _symtab.find_local(id);
    if (symbol) {
    	if (symbol->is_defined()) {
    	  throw id + " already defined";
      }

      for (size_t i = 0; i < symbol->args().size()  &&  i < args.size(); i++) {
    	   if(symbol->args().at(i)->name() != args.at(i)->name())
    	     throw id + " already declared with other types of arguments";
    	}

    	if(symbol->args().size() != args.size()) {
    	  throw id + " already declared with other types of arguments";
      }
    	 
    	if(symbol->type()->name() != node->return_type()->name())
    		throw id + " already declared with other return type";
    	
    	symbol->is_defined(true);
    } else {
    	std::shared_ptr<pwn::symbol> s = std::make_shared<pwn::symbol>(node->return_type(), node->name(), -1 ,true);
      s->args(args);
      s->is_var(false);
      s->offset(-1);
    	_symtab.insert(node->name(), s);
    }
}

void pwn::type_checker::do_function_call_node(pwn::function_call_node * const node, int lvl) {
  ASSERT_UNSPEC;

  std::shared_ptr<pwn::symbol> s;
  const std::string &id = node->name();
  
  if( (s = _symtab.find(id))) {
    const std::vector<basic_type*> args = s->args();
    cdk::expression_node *arg;
    
    if(node->arguments()!=NULL) {
        if(args.size() != node->arguments()->size()) {
            throw id + "called with wrong number of arguments";
        }
      
        for (size_t i = 0; i < args.size()  &&  i < node->arguments()->size(); i++) {
         arg = (cdk::expression_node*) node->arguments()->node(i);
         node->arguments()->node(i)->accept(this, lvl);
		      if(arg->type()->name() == basic_type::TYPE_UNSPEC  && args.at(i)->name() == basic_type::TYPE_INT) {
		      	arg->type(new basic_type(4, basic_type::TYPE_INT));
		      	continue;
		      } else if(arg->type()->name() == basic_type::TYPE_UNSPEC  &&  args.at(i)->name() == basic_type::TYPE_DOUBLE) {
		      	arg->type(new basic_type(8, basic_type::TYPE_DOUBLE));
		      	continue;
		      }
		
		
		      if (args.at(i)->name() != arg->type()->name()) {
		        throw id + ": argument types do not match!";
		      }
	      }
    }
  } else {
    throw id + " undeclared";
  }
    
    
  node->type(s->type());
}

void pwn::type_checker::do_memory_allocation_node(pwn::memory_allocation_node * const node, int lvl) {
  node->size()->accept(this, lvl + 2);

  //read node
  if (node->size()->type()->name() == basic_type::TYPE_UNSPEC) {
    node->size()->type(new basic_type(4, basic_type::TYPE_INT));
  }

  if (node->size()->type()->name() != basic_type::TYPE_INT) {
    throw "memory allocation must take an integer argument";
  }

  node->type(new basic_type(4, basic_type::TYPE_POINTER));
}


void pwn::type_checker::do_index_expression_node(pwn::index_expression_node * const node, int lvl) {

  node->shift()->accept(this, lvl + 2);
  if (node->shift()->type()->name() != basic_type::TYPE_INT) {
    throw "Index shift argument must be integer";  
  }
 
  node->expression()->accept(this, lvl + 2);

  if (node->expression()->type()->name() != basic_type::TYPE_POINTER) {
    throw "Index lvalue argument must be pointer";  
  }

  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

void pwn::type_checker::do_index_node(pwn::index_node * const node, int lvl) {

  node->expression()->accept(this, lvl + 2);

  if (node->expression()->type()->name() != basic_type::TYPE_POINTER) {
    throw "Index lvalue argument must be pointer";  
  }

  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));

}

void pwn::type_checker::do_address_node(pwn::address_node * const node, int lvl) {

  node->argument()->accept(this, lvl + 2);
  if ( ! (node->argument()->type()->name() == basic_type::TYPE_DOUBLE && 
     (node->argument()->name() == "simple_lvalue_node" || node->argument()->name() == "index_expression_node"))) {
    throw "argument of adressing operation must be a double lvalue";  
  }

  node->type(new basic_type(4, basic_type::TYPE_POINTER));
  
}

//void pwn::type_checker::do_and_node(pwn::and_node * const node, int lvl) {}
//void pwn::type_checker::do_address_node(pwn::address_node * const node, int lvl) {}
//void pwn::type_checker::do_function_call_node(pwn::function_call_node * const node, int lvl) {}
//void pwn::type_checker::do_function_declaration_node(pwn::function_declaration_node * const node, int lvl) {}
//void pwn::type_checker::do_function_definition_node(pwn::function_definition_node * const node, int lvl) {}
//void pwn::type_checker::do_identity_node(pwn::identity_node * const node, int lvl) {}
//void pwn::type_checker::do_next_node(pwn::next_node * const node, int lvl) {}
//void pwn::type_checker::do_or_node(pwn::or_node * const node, int lvl) {}
//void pwn::type_checker::do_repeat_node(pwn::repeat_node * const node, int lvl) {} FIXME
//void pwn::type_checker::do_return_node(pwn::return_node * const node, int lvl) {}
//void pwn::type_checker::do_stop_node(pwn::stop_node * const node, int lvl) {}
//void pwn::type_checker::do_not_node(pwn::not_node * const node, int lvl) {}
//void pwn::type_checker::do_index_node(pwn::index_node * const node, int lvl) {}
//void pwn::type_checker::do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl) {}
//void pwn::type_checker::do_memory_allocation_node(pwn::memory_allocation_node * const node, int lvl) {}
//void pwn::type_checker::do_noob_node(pwn::noob_node * const node, int lvl) {}
void pwn::type_checker::do_block_node(pwn::block_node * const node, int lvl) {}
//void pwn::type_checker::do_index_expression_node(pwn::index_expression_node * const node, int lvl) {}

