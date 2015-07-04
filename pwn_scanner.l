%option c++ yyclass="pwn_scanner" outfile="pwn_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
/* $Id: pwn_scanner.l,v 1.11 2015/05/19 00:56:07 ist175714 Exp $ */
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "pwn_scanner.h"
#include "pwn_parser.tab.h"

// don't change this
#define yyerror LexerError
%}

HEX     [0123456789a-fA-F]
PHEX    [123456789a-fA-F]

%x X_STRING X_COMMENT

%%

"//".*$                             ; /* ignore single line comments */

"/*"                                yy_push_state(X_COMMENT);
<X_COMMENT>"/*"                     yy_push_state(X_COMMENT);
<X_COMMENT>"*/"                     yy_pop_state();
<X_COMMENT>.|\n                     ; /* ignore block comments */


">="                                return tGE; 
"<="                                return tLE; 
"=="	                            return tEQ; 
"<>"                                return tNE; 

"repeat"                            return tREPEAT;
"next"                              return tNEXT;
"stop"                              return tSTOP;
"if"                                return tIF; 
"else"                              return tELSE; 
"!!"            	                return tPRINTLN; 
"local"                             return tLOCAL;
"import"                            return tIMPORT; 
"return"                            return tRETURN;
"noob"                              return tNOOB;


[[:alpha:]_][[:alnum:]_]*           yylval.s = new std::string(yytext); return tIDENTIFIER; 

\"                                  yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"						yy_pop_state(); return tSTRING;
<X_STRING>\\\"                      *yylval.s += yytext + 1;
<X_STRING>\\0{PHEX}                 *yylval.s += strtol(yytext+1, 0, 16);
<X_STRING>\\{PHEX}{HEX}?            *yylval.s += strtol(yytext+1, 0, 16);
<X_STRING>.                         *yylval.s += yytext;
<X_STRING>\n                        yyerror("newline in string");

[0-9]*"."[0-9]+"e"[+-]?[0-9]+       { yylval.d = strtod(yytext, nullptr); return tDOUBLE; }
[0-9]+"."[0-9]*"e"[+-]?[0-9]+       { yylval.d = strtod(yytext, nullptr); return tDOUBLE; }
[0-9]*"."[0-9]+                     { yylval.d = strtod(yytext, nullptr); return tDOUBLE; }
[0-9]+"."[0-9]*                     { yylval.d = strtod(yytext, nullptr); return tDOUBLE; }
[0-9]+"e"[+-]?[0-9]+                { yylval.d = strtod(yytext, nullptr); return tDOUBLE; }

"0x"{HEX}+                          { yylval.i = strtol(yytext, nullptr, 16); return tINTEGER; }
[1-9][0-9]*|0                       { yylval.i = strtol(yytext, nullptr, 10); return tINTEGER; }

[-()<>=+*/%;,~&|{}.!#*$?@\[\]]      return *yytext;

[ \t\n\r]+                          ; /* ignore whitespace */

.                                    yyerror("Unknown character");

%%

// Very, very dirty hack: flex is a mess generating C++ scanners.
int pwn_scanner::yywrap() { return 1; }
