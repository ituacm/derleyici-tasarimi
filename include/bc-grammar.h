/**
 * @file bc-grammar.h
 * @brief @ref bc_grammar_page
 */

#ifndef BC_GRAMMAR_H
#define BC_GRAMMAR_H


#include "tokenizer.h"

//! [Gramer declaration]
#include <rdesc/grammar.h>

/**_*/
#define BC_NT_COUNT 22
/**_*/
#define BC_MAX_ALT_COUNT 13
/**_*/
#define BC_MAX_ALT_SIZE 10

/** @brief Token ID. */
enum bc_tk_id {
	TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_PERCENT, TK_CARET,
	TK_PLUSEQ, TK_MINUSEQ, TK_STAREQ, TK_SLASHEQ, TK_PERCENTEQ, TK_CARETEQ, TK_EQ,
	TK_PLUSPLUS, TK_MINUSMINUS,
	TK_LT, TK_GT, TK_EQEQ, TK_EXCLEQ, TK_LTEQ, TK_GTEQ,
	TK_LPAREN, TK_RPAREN, TK_LCURLY, TK_RCURLY, TK_SEMI, TK_COMMA,
	TK_DEFINE, TK_BREAK, TK_QUIT, TK_PRINT,
	TK_RETURN, TK_FOR, TK_IF, TK_WHILE,
	/* TK_IDENT, TK_INT ve TK_FLOAT tokenizer tarafından tanımlanır */
};

/** @brief Nonterminal ID. */
enum bc_nt_id {
	NT_STMT, NT_STMTS,

	NT_ADD_OP, NT_MUL_OP,
	NT_ASGN_OP, NT_REL_OP,
	NT_UNARY_OP, NT_INCR_DECR_OP, NT_OPT_INCR_DECR_OP,

	NT_ADD_EXPR, NT_ADD_EXPR_REST,
	NT_MUL_EXPR, NT_MUL_EXPR_REST,
	NT_EXP_EXPR, NT_EXP_EXPR_REST,
	NT_FACTOR, NT_ATOM,
	NT_ASGN_EXPR, NT_REL_EXPR, NT_REL_EXPR_OPT_REL,
	NT_RETURN_EXPR,

	NT_EXPR,
};

/**_*/
/* Gramer tanımını tutacak struct'ı declare ediyoruz.*/
extern struct rdesc_grammar_symbol bc_production_rules
	[BC_NT_COUNT][BC_MAX_ALT_COUNT + 1][BC_MAX_ALT_SIZE + 1];
//! [Gramer declaration]

static const char *bc_tk_names[] = {
	"+", "-", "*", "/", "%", "^",
	"+=", "-=", "*=", "/=", "%=", "^=", "=",
	"++", "--",
	"<", ">", "==", "!=", "<=", ">=",
	"(", ")", "{", "}", ";", ",",
	"define", "break", "quit", "print",
	"return", "for", "if", "while",

	[TK_INT] = "@int", [TK_FLOAT] = "@float", [TK_IDENT] = "@ident",
};

static const char *bc_nt_names[] = {
	"stmt", "stmts",

	"add_op", "mul_op",
	"asgn_op", "rel_op",
	"unary_op", "incr_decr_op", "opt_incr_decr_op",

	"add_expr", "add_expr_rest",
	"mul_expr", "mul_expr_rest",
	"exp_expr", "exp_expr_rest",
	"factor", "atom",
	"asgn_expr", "rel_expr", "rel_expr_opt_rel",
	"return_expr",

	"expr",
};


#endif
