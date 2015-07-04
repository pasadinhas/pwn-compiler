// $Id: postfix_writer.cpp,v 1.21 2015/05/20 09:40:14 ist175714 Exp $ -*- c++ -*-
#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/enter_size.h"
#include "ast/all.h"  // all.h is automatically generated

//---------------------------------------------------------------------------
//     THIS IS THE VISITOR'S DEFINITION
//---------------------------------------------------------------------------

// Done
void pwn::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl + 1);
  }
  if (lvl == 0) {
    // Hack: executes once, at the end of all visits
    // Here we'll handle externs:
    // 1. If a function is declared but not defined nor used, we do nothing;
    // 2. If a function is declared and defined, there's nothing else to do;
    // 3. If a function is delcared, used but not defined, it's extern.

    // We do the intersection of _declared_functions and _called_functions
    
    std::vector<std::string *> intersection;
    
    for (std::vector<std::string *>::iterator id1 = _declared_functions.begin(); id1 != _declared_functions.end(); ++id1) {
      for (std::vector<std::string *>::iterator id2 = _called_functions.begin(); id2 != _called_functions.end(); ++id2) {
        if (**id1 == **id2) {
          bool duplicate = false;
          for (std::vector<std::string *>::iterator id3 = intersection.begin(); id3 != intersection.end(); ++id3) {
            if (**id1 == **id3) {
              duplicate = true;
              break;
            }
          }
          if ( ! duplicate) {
            intersection.push_back(*id1);            
          }
        }
      }
    }
    
    for (std::vector<std::string *>::iterator id = intersection.begin(); id != intersection.end(); ++id) {
      _pf.EXTERN(**id);
    }
  }
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  if (_current_function) {
    // local scope
    _pf.INT(node->value());
  } else {
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(*_current_id);
    _pf.CONST(node->value());
  }
}

// Done
void pwn::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  if (_current_function) {
    // local scope
    _pf.RODATA();
    _pf.ALIGN();
    _pf.LABEL(mklbl(++_lbl));
    _pf.DOUBLE(node->value());
    _pf.TEXT();
    _pf.ADDR(mklbl(_lbl));
    _pf.DLOAD();
  } else {
    // global scope
  	_pf.ALIGN();
  	_pf.LABEL(*_current_id);
    _pf.DOUBLE(node->value());
  }
}

// Done
void pwn::postfix_writer::do_noob_node(pwn::noob_node * const node, int lvl) { 
	CHECK_TYPES(_compiler, _symtab, node);
	
  if (_current_function) {
    _pf.INT(0);
  } else {
  	_pf.ALIGN();
  	_pf.LABEL(*_current_id);
    _pf.CONST(0);													
  }
}

// Done
void pwn::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.STR(node->value()); // output string characters

  if (_current_function) {
    // local scope
    _pf.TEXT(); // return to the TEXT segment
    _pf.ADDR(mklbl(lbl1)); // the string to be printed
  } else {
    // global scope
    _pf.ALIGN();
    _pf.LABEL(*_current_id); 
    _pf.ID(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  if (is_pointer(node->type())) {
    if (is_int(node->left()->type())) {
      node->right()->accept(this, lvl + 1); // load the pointer
      node->left()->accept(this, lvl + 1); // load the shift
    } else {
      node->left()->accept(this, lvl + 1); // load the pointer
      node->right()->accept(this, lvl + 1); // load the shift
    } 
    
    _pf.INT(4); // pointer size
    _pf.MUL(); // pointer arithmetics shift calculation
    _pf.ADD();
      
    return;
  }
  
  node->left()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->left()->type())) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->right()->type())) {
    _pf.I2D();
  }
  
  if (is_int(node->type())) {
    _pf.ADD();    
  } else if (is_double(node->type())) {
    _pf.DADD();
  }
}

void pwn::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
}

// Done - untested
void pwn::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->left()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->left()->type())) {
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->right()->type())) {
    _pf.I2D();
  }
  
  if (is_int(node->type())) {
    _pf.MUL();    
  } else if (is_double(node->type())) {
    _pf.DMUL();
  }
}

// Done - untested
void pwn::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->left()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->left()->type())) {
    _pf.DIV();
  }
  
  node->right()->accept(this, lvl);
  if (is_double(node->type()) && is_int(node->right()->type())) {
    _pf.I2D();
  }
  
  if (is_int(node->type())) {
    _pf.ADD();    
  } else if (is_double(node->type())) {
    _pf.DDIV();
  }
}

// Done
void pwn::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void pwn::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}
void pwn::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}
void pwn::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}
void pwn::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}
void pwn::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}
void pwn::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_rvalue_node(pwn::rvalue_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->lvalue()->accept(this, lvl);
  
  if (is_double(node->type())) {
    _pf.DLOAD();        
  } else {
    _pf.LOAD();    
  }
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_simple_lvalue_node(pwn::simple_lvalue_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  std::string id = fix_id(node->value());
  std::shared_ptr<pwn::symbol> symbol = _symtab.find(id); 
  
  if ( ! _current_function) {
    // global context
    if ( ! symbol->is_var()) {
      throw "Error: assigning a value to a function";
    }
    
    _pf.ADDR(id);
  } else {
    // local context
    bool is_return_symbol = ( ! symbol->is_var()) // the symbol is a function
                         && (_current_function->name() == symbol->name()) // the symbol has the same name as the current function
                         && ( ! is_void(_current_function->type())); // the function doesnt "return" void
                         
    if (is_return_symbol) {
      if (is_double(_current_function->type())) {
        _pf.LOCAL(-8); // double @ -8
      } else {
        _pf.LOCAL(-4); // int, string, pointer @ -4
      }
    } else if (symbol->is_var() && symbol->name() != _current_function->name()) {
      _pf.LOCAL(symbol->offset());
    } else {
      throw "Error: invalid left value";
    }
  }

}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_assignment_node(pwn::assignment_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  node->rvalue()->accept(this, lvl); // determine the new value
  _pf.DUP();
  node->lvalue()->accept(this, lvl); // where to store the value
  _pf.STORE(); // store the value at address
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_function_definition_node(pwn::function_definition_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  _current_function_end = ++_lbl;

  _pf.TEXT();
  _pf.ALIGN();
  std::string name = fix_id(node->name());
  
  // is global ?
  if ( ! node->is_local()) {
    _pf.GLOBAL(name, _pf.FUNC());    
  }
  
  _pf.LABEL(name);
  
  // find space needed for local variables
  pwn::enter_size *visitor = new pwn::enter_size(_compiler);
  node->accept(visitor, 0);
  int size = visitor->size();
  delete visitor;
  
  // reserve memory
  _pf.ENTER(size);

  // set current function
  _current_function = _symtab.find_local(node->name());
  
  // create new context
  _symtab.push();

  if (node->arguments()) {
    _current_offset = 8;  // function arguments offset
    node->arguments()->accept(this, lvl + 1);
  }
  
  _current_offset = 0; // local variables offset
  
  // reserve space for return variable
  int return_size = 0;
  if (is_int(node->return_type()) || is_string(node->return_type()) || is_pointer(node->return_type())) {
    return_size = 4; // int, string, pointer
  } else if (is_double(node->return_type())) {
    return_size = 8; // double
  }
  
  _current_offset -= return_size;
  
  // handle default return value
  if (node->return_default()) {
    node->return_default()->accept(this, lvl + 1);
    if (return_size == 4) {
      _pf.LOCA(-4);
    } else if (return_size == 8) {
      _pf.LOCAL(-8);
      _pf.DSTORE();
    }
  } else {
    // if no return default is specified, return 0 if int or pointer
    _pf.INT(0);
    _pf.LOCA(-4);
  }
  
  // function body
  node->block()->accept(this, lvl);
  
  _pf.ALIGN();
  _pf.LABEL(mklbl(_current_function_end));

  // put return value in eax
  if (return_size == 4) {
    // int string pointer
    _pf.LOCV(-4);
    _pf.POP();
  } else if (return_size == 8) {
    // double
    _pf.LOCAL(-8);
    _pf.DLOAD();
    _pf.DPOP();
  }

  // return from function
  _pf.LEAVE();
  _pf.RET();
  
  // leave local context
  _symtab.pop(); 
  _current_function = nullptr;

  // add this function to defined functions list
  _defined_functions.push_back(new std::string(name));
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_evaluation_node(pwn::evaluation_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->argument()->accept(this, lvl); // determine the value
  _pf.TRASH(node->argument()->type()->size()); // trash the result
}

void pwn::postfix_writer::do_print_node(pwn::print_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->argument()->accept(this, lvl); // determine the value to print
  if (is_int(node->argument()->type())) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
    std::string *f_name = new std::string("printi");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
  }
  else if (is_string(node->argument()->type())) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
    std::string *f_name = new std::string("prints");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
  } else if (is_double(node->argument()->type())) {
    _pf.CALL("printd");
    _pf.TRASH(8); // delete double printed value
    std::string *f_name = new std::string("printd");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
  } else {
    std::cerr << "ERROR: Trying to print invalid type." << std::endl;
    exit(1);
  }
  if (node->new_line()) {
    _pf.CALL("println"); // print a newline
    std::string *f_name = new std::string("println");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
  }
}

//---------------------------------------------------------------------------

void pwn::postfix_writer::do_read_node(pwn::read_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  if (is_int(node->type()) || node->type()->name() == basic_type::TYPE_UNSPEC) {
    _pf.CALL("readi");
    std::string *f_name = new std::string("readi");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
    _pf.PUSH();
  } else {
    _pf.CALL("readd");
    std::string *f_name = new std::string("readd");
    _declared_functions.push_back(f_name);
    _called_functions.push_back(f_name);
    _pf.DPUSH();
  }
}

//---------------------------------------------------------------------------

// Legacy
void pwn::postfix_writer::do_while_node(cdk::while_node * const node, int lvl) {}

// Maybe done - i think it's done but it's untested
void pwn::postfix_writer::do_repeat_node(pwn::repeat_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  int lbl1 = ++_lbl;
  int lbl2 = ++_lbl; _repeat_increment.push_back(lbl2);
  int lbl3 = ++_lbl; _repeat_end.push_back(lbl3);
  
  if (node->inicializations()) {
    node->inicializations()->accept(this, lvl); // init repeat    
  }
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1)); // condition label
  node->conditions()->accept(this, lvl);
  _pf.JZ(mklbl(lbl3)); // jump if condition is false
  node->block()->accept(this, lvl + 2);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl2)); // increment label
  node->updates()->accept(this, lvl);
  _pf.JMP(mklbl(lbl1)); // go to condition label
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl3)); // end cycle
  
  _repeat_increment.pop_back();
  _repeat_end.pop_back();
}

//---------------------------------------------------------------------------

// Done
void pwn::postfix_writer::do_if_node(cdk::if_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

// Done
void pwn::postfix_writer::do_if_else_node(cdk::if_else_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//----------------------------------------------------------------------------

// Done
void pwn::postfix_writer::do_and_node(pwn::and_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  int lbl0 = ++_lbl;
  int lbl_end = ++_lbl;
  
  node->left()->accept(this, lvl + 1);
  _pf.JZ(mklbl(lbl0));
  node->right()->accept(this, lvl + 1);
  _pf.JZ(mklbl(lbl0));
  _pf.INT(1);
  _pf.JMP(mklbl(lbl_end));
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl0));
  _pf.INT(0);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

// Done
void pwn::postfix_writer::do_or_node(pwn::or_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  int lbl1 = ++_lbl;
  int lbl_end = ++_lbl;
  
  node->left()->accept(this, lvl + 1);
  _pf.JNZ(mklbl(lbl1));
  node->right()->accept(this, lvl + 1);
  _pf.JNZ(mklbl(lbl1));
  _pf.INT(0);
  _pf.JMP(mklbl(lbl_end));
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
  _pf.INT(1);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

// Done
void pwn::postfix_writer::do_not_node(pwn::not_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  int lbl1 = ++_lbl;
  int lbl_end = ++_lbl;
  
  node->argument()->accept(this, lvl + 1);
  _pf.JZ(mklbl(lbl1));
  _pf.INT(0);
  _pf.JMP(mklbl(lbl_end));
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1));
  _pf.INT(1);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

// Done
void pwn::postfix_writer::do_identity_node(pwn::identity_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->argument()->accept(this, lvl);
}

// Done
void pwn::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->argument()->accept(this, lvl); // determine the value
  if (is_int(node->argument()->type())) {
    _pf.NEG(); // 2-complement    
  } else {
    // Double
    _pf.DNEG();
  }
}

void pwn::postfix_writer::do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  std::string id = fix_id(node->name());
  bool initialize = node->initial_value() != nullptr; 
  std::shared_ptr<pwn::symbol> symbol = _symtab.find(id);
  
  if (_current_function) 
  { // local scope
    if (node->is_local() || node->is_import()) {
      throw "It's not allowed to use local or import keywords in blocks.";
    }
    
    if (_current_offset >= 8) {
      // function arguments
      if (initialize) {
        throw "Error: function arguments cannot be initialized.";
      }
      symbol->offset(_current_offset);
      _current_offset += node->type()->size();
    } else if (_current_offset <= 0) {
      // local variables
      _current_offset -= node->type()->size();
      symbol->offset(_current_offset);
      if (initialize) {
        node->initial_value()->accept(this, lvl);
        _pf.LOCAL(symbol->offset());
        
        basic_type *var_type = node->type();
        if (is_int(var_type) || is_string(var_type) || is_pointer(var_type)) {
          _pf.STORE();
        } else if (is_double(var_type)) {
          _pf.DSTORE();
        }
      } // end initialize
    } else {
      throw "Error: bad offset.";
    }
    
  } // end local scope
  else 
  { // global scope 
    if (node->is_import()) {
      _pf.EXTERN(id);
      return; // nothing else to do.
    }
    
    // Choose segment
    if (initialize) {
      _pf.DATA();
    } else {
      _pf.BSS();
    }
    
    if ( ! node->is_local()) {
      _pf.GLOBAL(node->name(), _pf.OBJ());
    }
      
    if (initialize) {
      _current_id = &id;
      node->initial_value()->accept(this, lvl);
    } else {
      _pf.ALIGN();
      _pf.LABEL(id);
      _pf.BYTE(node->type()->size());
    }
  } // end global scope
  
}

// Done
void pwn::postfix_writer::do_block_node(pwn::block_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  _symtab.push();
  if (node->declarations())
    node->declarations()->accept(this, lvl);
  if (node->statements())
    node->statements()->accept(this, lvl);
  _symtab.pop();
}

void pwn::postfix_writer::do_address_node(pwn::address_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->argument()->accept(this, lvl);
}

void pwn::postfix_writer::do_index_node(pwn::index_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  node->expression()->accept(this, lvl); // get base address
  _pf.INT(node->shift());
  _pf.INT(8); // size of double
  _pf.MUL();
  _pf.ADD();
}

void pwn::postfix_writer::do_function_call_node(pwn::function_call_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  std::string id = fix_id(node->name());
  std::shared_ptr<pwn::symbol> symbol = _symtab.find(id);
  
  // put arguments in stack (reverse order)
  int args_size = 0;
  if (node->arguments()) {
    for (int i = node->arguments()->size() - 1; i >= 0; i--) {
      node->arguments()->node(i)->accept(this, lvl);
      args_size += ((cdk::expression_node *) node->arguments()->node(i))->type()->size();
    }
  }
  
  // call function
  _pf.CALL(id);
  
  // remove arguments from stack
  _pf.TRASH(args_size);
  
  // put return value in the stack
  if (is_int(symbol->type()) || is_pointer(symbol->type()) || is_string(symbol->type())) {
    _pf.PUSH();
  } else if (is_double(symbol->type())) {
    _pf.DPUSH();
  }
  
  // add this function to called functions
  _called_functions.push_back(new std::string(id));
}

void pwn::postfix_writer::do_function_declaration_node(pwn::function_declaration_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  std::string id = fix_id(node->name());
  
  // add this function to declared functions
  _declared_functions.push_back(new std::string(id));
}

void pwn::postfix_writer::do_memory_allocation_node(pwn::memory_allocation_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->size()->accept(this, lvl); // get number of doubles to alloc  
  _pf.INT(8);                      // size of double
  _pf.MUL();  
  _pf.ALLOC();
  _pf.SP();                        // leaves the stack pointer - address of array
}

void pwn::postfix_writer::do_index_expression_node(pwn::index_expression_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);
  
  node->expression()->accept(this, lvl); // get base address
  node->shift()->accept(this, lvl);      // calculate shift positions 
  _pf.INT(8);                            // size of double
  _pf.MUL();
  _pf.ADD();
  _pf.DLOAD();
}

void pwn::postfix_writer::do_next_node(pwn::next_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  unsigned int n = node->value();
  int lbl;
  
  if (n > _repeat_end.size()) {
    throw std::string("Error: there are not enough active cycles");
  }
  
  for (unsigned int i = 0; i < n; i++) {
    lbl = _repeat_increment.back();
    _repeat_increment.pop_back();
    _repeat_end.pop_back();
  }
  
  _pf.JMP(mklbl(lbl));
}
void pwn::postfix_writer::do_stop_node(pwn::stop_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  unsigned int n = node->value();
  int lbl;
  
  if (n > _repeat_end.size()) {
    throw std::string("Error: there are not enough active cycles");
  }
  
  for (unsigned int i = 0; i < n; i++) {
    lbl = _repeat_end.back();
    _repeat_increment.pop_back();
    _repeat_end.pop_back();
  }
  
  _pf.JMP(mklbl(lbl));
}

void pwn::postfix_writer::do_return_node(pwn::return_node * const node, int lvl) {
  CHECK_TYPES(_compiler, _symtab, node);

  _pf.JMP(mklbl(_current_function_end));
}