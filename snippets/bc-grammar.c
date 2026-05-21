#include "../include/bc-grammar.h"
#include "../include/tokenizer.h"  // IWYU pragma: keep (TK_*)

#include <rdesc/rule_macros.h>


//! [Gramer tanımı]
struct rdesc_grammar_symbol bc_production_rules
	[BC_NT_COUNT][BC_MAX_ALT_COUNT + 1][BC_MAX_ALT_SIZE + 1] = {
/* <stmt> ::= */ r(
	NT(EXPR), TK(SEMI)
alt	TK(BREAK), TK(SEMI)
alt	TK(QUIT), TK(SEMI)
alt	TK(RETURN), NT(OPT_EXPR), TK(SEMI)
alt	TK(FOR), TK(LPAREN), NT(OPT_EXPR), TK(SEMI),
			     NT(OPT_EXPR), TK(SEMI),
			     NT(OPT_EXPR),
		TK(RPAREN), NT(STMT)
alt	TK(IF), TK(LPAREN), NT(REL_EXPR), TK(RPAREN), NT(STMT)
alt	TK(WHILE), TK(LPAREN), NT(REL_EXPR), TK(RPAREN), NT(STMT)
alt	TK(PRINT), NT(EXPR), TK(SEMI)
alt	TK(LCURLY), NT(STMTS), TK(RCURLY)
alt	TK(SEMI)
),

/* <stmts> ::= */ r(
	NT(STMT), NT(STMTS)
alt	EPSILON
),

/* <add-op> ::= */ r(
	TK(PLUS)
alt	TK(MINUS)
),
/* <mul-op> ::= */ r(
	TK(STAR)
alt	TK(SLASH)
alt	TK(PERCENT)
),
/* <asgn-op> ::= */ r(
	TK(PLUSEQ)
alt	TK(MINUSEQ)
alt	TK(STAREQ)
alt	TK(SLASHEQ)
alt	TK(PERCENTEQ)
alt	TK(CARETEQ)
alt	TK(EQ)
),
/* <rel-op> ::= */ r(
	TK(LT)
alt	TK(GT)
alt	TK(EQEQ)
alt	TK(EXCLEQ)
alt	TK(LTEQ)
alt	TK(GTEQ)
),
/* <unary-op> ::= */ r(
	TK(PLUS)
alt	TK(MINUS)
alt	EPSILON
),
/* <incr-decr-op> ::= */ r(
	TK(PLUSPLUS)
alt	TK(MINUSMINUS)
),
/* <opt-incr-decr-op> ::= */ r(
	TK(PLUSPLUS)
alt	TK(MINUSMINUS)
alt	EPSILON
),

/* <add-expr> ::= */
	rrr(ADD_EXPR, (NT(MUL_EXPR)), (NT(ADD_OP), NT(MUL_EXPR))),

/* <mul-expr> ::= */
	rrr(MUL_EXPR, (NT(EXP_EXPR)), (NT(MUL_OP), NT(EXP_EXPR))),

/* <exp-expr> ::= */
	rrr(EXP_EXPR, (NT(FACTOR)), (TK(CARET), NT(FACTOR))),

/* <factor> ::= */ r(
	NT(UNARY_OP), NT(ATOM)
),

/* <atom> ::= */ r(
	NT(INCR_DECR_OP), TK(IDENT)
alt	TK(IDENT), NT(OPT_INCR_DECR_OP)
alt	TK(LPAREN), NT(EXPR), TK(RPAREN)
alt	TK(INT)
alt	TK(FLOAT)
),

/* <asgn-expr> ::= */ r(
	TK(IDENT), NT(ASGN_OP), NT(ADD_EXPR)
),

/* <rel-expr> ::= */ r(
	NT(ADD_EXPR), NT(REL_EXPR_OPT_REL)
),

/* <rel-expr-opt-rel> ::= */
	ropt(NT(REL_OP), NT(ADD_EXPR)),

/* <opt-expr> ::= */
	ropt(NT(EXPR)),

/* <expr> */ r(
	NT(ASGN_EXPR)
alt	NT(REL_EXPR)
)
};
//! [Gramer tanımı]

//! [Tokenizer init]
void bc_tokenizer_init(struct tokenizer *t)
{
	tokenizer_init(t);

	for (int i = TK_PLUS; i <= TK_COMMA; i++)
		tokenizer_add_punctuation(t, bc_tk_names[i], i);

	for (int i = TK_DEFINE; i <= TK_WHILE; i++)
		tokenizer_add_keyword(t, bc_tk_names[i], i);
}
//! [Tokenizer init]
