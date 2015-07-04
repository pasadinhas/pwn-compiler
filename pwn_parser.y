%{
// $Id: pwn_parser.y,v 1.16 2015/05/19 11:34:58 ist176012 Exp $
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE       compiler->scanner()->lineno()
#define yylex()    compiler->scanner()->scan()
#define yyerror(s) compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!

#define CONST_BIT 1UL<<8
%}

%union {
  int                   i;	/* integer value */
  double                d;
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  pwn::lvalue_node     *lvalue;
  cdk::identifier_node *identifier;
  pwn::block_node      *block;
  basic_type           *type;
  cdk::string_node     *strnode;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tWHILE tIF tPRINT tPRINTLN tLOCAL tIMPORT tRETURN tNOOB tREAD tREPEAT tNEXT tSTOP

%nonassoc tIFX
%nonassoc tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%left '&' '|'
%nonassoc tUNARY '?' '~' '[' ']'

%type <node> stmt decl var func
%type <block> block
%type <sequence> list decls exprs vars
%type <expression> expr literal
%type <lvalue> lval 
%type <type> type
%type <strnode> strnode
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : decls                { compiler->ast($1); }
     ;

decls : decl                { $$ = new cdk::sequence_node(LINE, $1); }
      | decls decl          { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

decl : var ';'              { $$ = $1; }
     | func                 { $$ = $1; }
     ;

type : '<' '#' '>'         { $$ = new basic_type(4, basic_type::TYPE_INT | CONST_BIT); }
     | '<' '%' '>'         { $$ = new basic_type(8, basic_type::TYPE_DOUBLE | CONST_BIT); }
     | '<' '$' '>'         { $$ = new basic_type(4, basic_type::TYPE_STRING | CONST_BIT); }
     | '<' '*' '>'         { $$ = new basic_type(4, basic_type::TYPE_POINTER | CONST_BIT); }
     | '#'                 { $$ = new basic_type(4, basic_type::TYPE_INT); }
     | '%'                 { $$ = new basic_type(4, basic_type::TYPE_DOUBLE); }
     | '$'                 { $$ = new basic_type(4, basic_type::TYPE_STRING); }
     | '*'                 { $$ = new basic_type(4, basic_type::TYPE_POINTER); }
     | '!'                 { $$ = new basic_type(0, basic_type::TYPE_VOID); }
     ;

var : type tIDENTIFIER                       { $$ = new pwn::variable_declaration_node(LINE, $2, $1); }
    | type tIDENTIFIER '=' expr              { $$ = new pwn::variable_declaration_node(LINE, $2, $1, $4); }
    | tLOCAL type tIDENTIFIER                { $$ = new pwn::variable_declaration_node(LINE, $3, $2, nullptr, true); }
    | tLOCAL type tIDENTIFIER '=' expr       { $$ = new pwn::variable_declaration_node(LINE, $3, $2, $5, true); } 
    | tIMPORT type tIDENTIFIER               { $$ = new pwn::variable_declaration_node(LINE, $3, $2, nullptr, false, true); }
    ;

func : tIMPORT type tIDENTIFIER '(' vars ')'                     { $$ = new pwn::function_declaration_node(LINE, $3, $5, $2, false, true); }
     | type tIDENTIFIER '(' vars ')' block                       { $$ = new pwn::function_definition_node(LINE, $2, $4, $1, $6); } 
     | type tIDENTIFIER '(' vars ')'                             { $$ = new pwn::function_declaration_node(LINE, $2, $4, $1); }
     | tLOCAL type tIDENTIFIER '(' vars ')' block                { $$ = new pwn::function_definition_node(LINE, $3, $5, $2, $7, nullptr, true); } 
     | tLOCAL type tIDENTIFIER '(' vars ')'                      { $$ = new pwn::function_declaration_node(LINE, $3, $5, $2, true); }
     | type tIDENTIFIER '(' vars ')' '=' literal block           { $$ = new pwn::function_definition_node(LINE, $2, $4, $1, $8, $7); } 
     | tLOCAL type tIDENTIFIER '(' vars ')' '=' literal block    { $$ = new pwn::function_definition_node(LINE, $3, $5, $2, $9, $8, true); } 
     ;

literal : tINTEGER    { $$ = new cdk::integer_node(LINE, $1); }
        | tDOUBLE     { $$ = new cdk::double_node(LINE, $1); }
        | strnode     { $$ = $1; }
        | tNOOB       { $$ = new pwn::noob_node(LINE); }
        ;

vars : vars ',' var         { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | var                  { $$ = new cdk::sequence_node(LINE, $1); }
     |                      { $$ = nullptr; }
     ;
     
block : '{' '}'                  { $$ = new pwn::block_node(LINE); }
      | '{' decls '}'            { $$ = new pwn::block_node(LINE, $2); }
      | '{' list '}'             { $$ = new pwn::block_node(LINE, nullptr, $2); }
      | '{' decls list '}'       { $$ = new pwn::block_node(LINE, $2, $3);}
      ;

list : stmt                  { $$ = new cdk::sequence_node(LINE, $1); }
	   | list stmt          { $$ = new cdk::sequence_node(LINE, $2, $1); }
	   ;

stmt : expr ';'                                             { $$ = new pwn::evaluation_node(LINE, $1); }
     | expr tPRINTLN                                        { $$ = new pwn::print_node(LINE, $1, true); }
     | expr '!'                                             { $$ = new pwn::print_node(LINE, $1); } 
     | tIF '(' expr ')' stmt %prec tIFX                     { $$ = new cdk::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt tELSE stmt                     { $$ = new cdk::if_else_node(LINE, $3, $5, $7); }
     | tREPEAT '(' exprs ';' exprs ';' exprs ')' stmt       { $$ = new pwn::repeat_node(LINE, $3, $5, $7, $9); }
     | block                                                { $$ = $1; }
     | tSTOP ';'                                            { $$ = new pwn::stop_node(LINE); }
     | tSTOP tINTEGER ';'                                   { $$ = new pwn::stop_node(LINE, $2); }
     | tNEXT ';'                                            { $$ = new pwn::next_node(LINE); }
     | tNEXT tINTEGER ';'                                   { $$ = new pwn::next_node(LINE, $2); }
     | tRETURN                                              { $$ = new pwn::return_node(LINE); }
     ;

exprs : expr                   { $$ = new cdk::sequence_node(LINE, $1); }
      | exprs ',' expr         { $$ = new cdk::sequence_node(LINE, $3, $1); }
      |                        { $$ = nullptr; }
      ; 

expr : literal                    { $$ = $1; }
     | '-' expr %prec tUNARY      { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY      { $$ = new pwn::identity_node(LINE, $2); }
     | expr '?'                   { $$ = new pwn::address_node(LINE, $1); }
     | '~' expr                   { $$ = new pwn::not_node(LINE, $2); }
     | expr '&' expr              { $$ = new pwn::and_node(LINE, $1, $3); }
     | expr '|' expr              { $$ = new pwn::or_node(LINE, $1, $3); }
     | expr '+' expr	         { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	         { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	         { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	         { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	         { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr	         { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	         { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	         { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr              { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	         { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	         { $$ = new cdk::eq_node(LINE, $1, $3); }
     | '(' expr ')'               { $$ = $2; }
     | lval                       { $$ = new pwn::rvalue_node(LINE, $1); }
     | lval '=' expr              { $$ = new pwn::assignment_node(LINE, $1, $3); }
     | expr '[' expr ']'          { $$ = new pwn::index_expression_node(LINE, $1, $3); }
     | '[' expr ']'           { $$ = new pwn::memory_allocation_node(LINE, $2); }
     | '@'                        { $$ = new pwn::read_node(LINE);}
     | tIDENTIFIER '(' exprs ')'  { $$ = new pwn::function_call_node(LINE, $1, $3); }
     ;

lval : tIDENTIFIER                { $$ = new pwn::simple_lvalue_node(LINE, $1); }
     | expr '[' tINTEGER ']'      { $$ = new pwn::index_node(LINE, $1, $3); }
     ;

strnode : string                        { $$ = new cdk::string_node(LINE, $1); }
        ;

string : string tSTRING                 { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
       | tSTRING                        { $$ = $1; }
       ;

%%
