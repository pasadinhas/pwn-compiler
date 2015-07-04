#include <string>
#include "targets/enter_size.h"
#include "ast/all.h"  // all.h is automatically generated

void pwn::enter_size::do_sequence_node(cdk::sequence_node * const node, int lvl) {
	for (size_t i = 0; i < node->size(); i++) {
		node->node(i)->accept(this, lvl);
	}
}

void pwn::enter_size::do_function_definition_node(pwn::function_definition_node * const node, int lvl) {
	node->block()->accept(this, lvl);
}

void pwn::enter_size::do_variable_declaration_node(pwn::variable_declaration_node * const node, int lvl) {
	_size += (node->type()->name() == basic_type::TYPE_DOUBLE) ? 8 : 4;
}

void pwn::enter_size::do_block_node(pwn::block_node * const node, int lvl) {
	if (node->declarations())
	    node->declarations()->accept(this, lvl);
	if (node->statements())
		node->statements()->accept(this, lvl);
}

void pwn::enter_size::do_if_node(cdk::if_node * const node, int lvl) {
	node->block()->accept(this, lvl);
}

void pwn::enter_size::do_if_else_node(cdk::if_else_node * const node, int lvl) {
	node->thenblock()->accept(this, lvl);	
	node->elseblock()->accept(this, lvl);	
}

void pwn::enter_size::do_repeat_node(pwn::repeat_node * const node, int lvl) {
	node->block()->accept(this, lvl);
}