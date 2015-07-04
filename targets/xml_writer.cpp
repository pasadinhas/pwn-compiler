// $Id: xml_writer.cpp,v 1.13 2015/05/19 20:29:57 ist176012 Exp $ -*- c++ -*-
#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//---------------------------------------------------------------------------

void pwn::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  processSimple(node, lvl);
}

void pwn::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  processSimple(node, lvl);
}

void pwn::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  processSimple(node, lvl);
}

//---------------------------------------------------------------------------

inline void pwn::xml_writer::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

inline void pwn::xml_writer::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void pwn::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_rvalue_node(pwn::rvalue_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_simple_lvalue_node(pwn::simple_lvalue_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  //processSimple(node, lvl);
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<identifier>" << node->value() << "</identifier>" << std::endl; 
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_assignment_node(pwn::assignment_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  openTag("rvalue", lvl + 2);
  node->rvalue()->accept(this, lvl + 4);
  closeTag("rvalue", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_evaluation_node(pwn::evaluation_node * const node, int lvl) {
  ////CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_print_node(pwn::print_node * const node, int lvl) {
  //CHECK_TYPES(_compiler, _symtab, node);
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_read_node(pwn::read_node * const node, int lvl) {
  processJustTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_while_node(cdk::while_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_repeat_node(pwn::repeat_node * const node, int lvl) {
  openTag(node, lvl);
  doCanBeNull("arguments", node->inicializations(), lvl + 2);
  doCanBeNull("conditions", node->conditions(), lvl + 2);
  doCanBeNull("updates", node->updates(), lvl + 2);
  openTag("statement", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("statement", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void pwn::xml_writer::do_if_node(cdk::if_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_if_else_node(cdk::if_else_node * const node, int lvl) {
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

//-------------------------------------------------------------------------------

void pwn::xml_writer::do_and_node(pwn::and_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

void pwn::xml_writer::do_or_node(pwn::or_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//-------------------------------------------------------------------------------

void pwn::xml_writer::do_address_node(pwn::address_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void pwn::xml_writer::do_identity_node(pwn::identity_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void pwn::xml_writer::do_not_node(pwn::not_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}


//-------------------------------------------------------------------------------

void pwn::xml_writer::do_next_node(pwn::next_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->name() << ">" << node->value() << "</" << node->name() << ">" << std::endl;
}

void pwn::xml_writer::do_return_node(pwn::return_node * const node, int lvl) {
  processJustTag(node, lvl);
}

void pwn::xml_writer::do_stop_node(pwn::stop_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->name() << ">" << node->value() << "</" << node->name() << ">" << std::endl;
}

//-------------------------------------------------------------------------------

void pwn::xml_writer::do_function_call_node(pwn::function_call_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<name>" << node->name() << "</name>" << std::endl; 
  doCanBeNull("arguments", node->arguments(), lvl + 2);
  closeTag(node, lvl);
}

//-------------------------------------------------------------------------------

void pwn::xml_writer::do_function_declaration_node(pwn::function_declaration_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<name>" << node->name() << "</name>" << std::endl; 
  doCanBeNull("arguments", node->arguments(), lvl + 2);
  processReturnType(node->return_type(), lvl + 2);
  closeTag(node, lvl);
}
void pwn::xml_writer::do_function_definition_node(pwn::function_definition_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<name>" << node->name() << "</name>" << std::endl; 
  doCanBeNull("arguments", node->arguments(), lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  processReturnType(node->return_type(), lvl + 2);
  closeTag(node, lvl);
}


void pwn::xml_writer::do_index_node(pwn::index_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<shift>" << node->shift() << "</shift>" << std::endl; 
  closeTag(node, lvl);
}
void pwn::xml_writer::do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<name>" << node->name() << "</name>" << std::endl;
  doCanBeNull("initial_value", node->initial_value(), lvl + 2);
  processVarType(node->type(), lvl + 2);
  closeTag(node, lvl);
}
void pwn::xml_writer::do_memory_allocation_node(pwn::memory_allocation_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl+2, ' ') << "<" << node->size() << ">" << node->size() << "</" << node->name() << ">" << std::endl;
  closeTag(node, lvl);
}

void pwn::xml_writer::do_noob_node(pwn::noob_node * const node, int lvl) {
  processJustTag(node, lvl);
}

void pwn::xml_writer::do_block_node(pwn::block_node * const node, int lvl) {
  openTag(node, lvl);
  doCanBeNull("declarations", node->declarations(), lvl + 2);
  doCanBeNull("statements", node->statements(), lvl + 2);
  closeTag(node, lvl);
}

void pwn::xml_writer::do_index_expression_node(pwn::index_expression_node * const node, int lvl) {
  openTag(node, lvl);
  os() << std::string(lvl + 2, ' ') << "<identifier>" << node->name() << "</identifier>" << std::endl;
  openTag("shift", lvl+2);
  node->shift()->accept(this, lvl + 4);
  closeTag("shift", lvl+2);
  closeTag(node, lvl);
}
