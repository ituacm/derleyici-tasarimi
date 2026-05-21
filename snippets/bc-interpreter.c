#include "../include/bc-grammar.h"
#include "../include/bc-interpreter.h"
#include "../include/tokenizer.h"

#include "../include/map.h"

#include <rdesc/grammar.h>
#include <rdesc/rdesc.h>

#include <assert.h>
#include <rdesc/util.h>
#include <stdio.h>
#include <string.h>


//! [bc_cli]
void bc_cli()
{
	struct rdesc_grammar g;  /* (grammar) */
	struct rdesc p;  /* (parser) */
	struct map v;  /* (variables) */
	struct tokenizer t;  /* (tokenizer) */

	/* librdesc gramerine bc_production_rules'ı yükle */
	assert(rdesc_grammar_init_checked(&g,
		BC_NT_COUNT, BC_MAX_ALT_COUNT, BC_MAX_ALT_SIZE,
		bc_production_rules) == 0);

	/* bu gramerle parser initialize et */
	assert(rdesc_init(&p, &g, sizeof(union seminfo), NULL) == 0);

	/* değişkenlerin map'ini initialize et */
	map_init(&v, sizeof(double));

	/* keyword ve punctuatoin'ları kaydettiğimiz tokenizer */
	bc_tokenizer_init(&t);

	assert(rdesc_start(&p, NT_STMT) == 0);

	struct lexer l;
	char buf[4096];

	enum stmt_meta ret;
	bool quit = false;

	while ((fgets(buf, 4096, stdin)) != NULL && !quit) {
		lexer_init(&l, buf);

		struct token tk;

		do {
			tk = tokenizer_next(&t);

			while (tk.id == TK_NOTOKEN) {
				struct lexeme next_lexeme = lexer_next(&l);

				if (next_lexeme.kind == LEXEME_EOF) {
					tk.id = TK_NOTOKEN;

					break;
				} else {
					tokenizer_feed(&t, next_lexeme);

					tk = tokenizer_next(&t);
				}
			}

			/* Bir sonraki tokeni çekebildiğimiz durumda, onu
			 * rdesc'e göderiyoruz. */

			if (tk.id != TK_NOTOKEN) {
				switch (rdesc_pump(&p, tk.id, &tk.seminfo)) {
				case RDESC_READY:
					ret = bc_tw_eval_stmt(&v,
							rdesc_get_root(&p));

					quit = ret == QUIT;

					assert(ret != BREAK && "Yakalanmamış break.");

					assert(rdesc_start(&p, NT_STMT) == 0);

					break;

				case RDESC_CONTINUE:
					break;

				case RDESC_NOMATCH:
					fprintf(stdout, "Syntax error!\n");

					rdesc_reset(&p);
					assert(rdesc_start(&p, NT_STMT) == 0);

					break;

				case RDESC_ENOMEM:
					assert(0 && "nomem");
				}
			}
		} while (tk.id != TK_NOTOKEN && !quit);
	}

	tokenizer_destroy(&t);
	rdesc_destroy(&p);
	rdesc_grammar_destroy(&g);
	map_destroy(&v);
}
//! [bc_cli]

//! [bc_tw_eval_stmt]
#include <rdesc/cst_macros.h>


enum stmt_meta bc_tw_eval_stmt(struct map *v, struct rdesc_node n)
{
	enum stmt_meta ret;

	switch (rid(n)) {
	case NT_STMT:
		switch (ralt_idx(n)) {
		case 0:  /* <expr> ; */
			bc_tw_eval_expr(v, rchild(n, 0));

			return VOID;

		case 1:  /* break ; */
			return BREAK;

		case 2:  /* quit ; */
			return QUIT;

		case 3:  /* return <opt-expr> ; */
			assert(0 && "Fonksiyonlar implement edilmedi.");

		case 4:  /* for ( <opt-expr> ; <opt-expr> ; <opt-expr> ) <stmt> */
			for (bc_tw_eval_expr(v, rchild(n, 2));
			     bc_tw_eval_expr(v, rchild(n, 4)) != 0;
			     bc_tw_eval_expr(v, rchild(n, 6))) {
				ret = bc_tw_eval_stmt(v, rchild(n, 8));

				if (ret == BREAK)
					break;

				if (ret == QUIT)
					return ret;
			}

			return VOID;

		case 5:  /* if ( <rel-expr> ) <stmt> */
			if (bc_tw_eval_expr(v, rchild(n, 2)) != 0)
				return bc_tw_eval_stmt(v, rchild(n, 4));

			return VOID;

		case 6: {  /* while ( <rel-expr> ) <stmt> */
			while (bc_tw_eval_expr(v, rchild(n, 2)) != 0) {
				ret = bc_tw_eval_stmt(v, rchild(n, 4));

				if (ret == BREAK)
					break;

				if (ret == QUIT)
					return ret;
			}

			return VOID;
		}

		case 7:  /* print <expr> ; */
			printf(">> %g\n", bc_tw_eval_expr(v, rchild(n, 1)));

			return VOID;

		case 8:  /* { <stmts> } */
			return bc_tw_eval_stmt(v, rchild(n, 1));

		case 9: /* ; */
			return VOID;
		}

		break;

	case NT_STMTS:
		switch (ralt_idx(n)) {
		case 0:  /* <stmt> <stmts> */
			ret = bc_tw_eval_stmt(v, rchild(n, 0));

			if (ret != VOID)
				return ret;

			return bc_tw_eval_stmt(v, rchild(n, 1));

		default:  /* ε */
			return VOID;
		}

		break;
	}

	assert(0 && "Yakalanmamış statement."); // GCOVR_EXCL_LINE: unreachable
}
//! [bc_tw_eval_stmt]


//! [bc_tw_eval_expr]
#include <math.h>

#define flip_if_not_flipped(parent, child_idx) \
	if (rchild_count(rchild(parent, child_idx)) == 2) \
		rdesc_flip_left(parent, child_idx);

double bc_tw_eval_expr(struct map *v, struct rdesc_node n)
{
	union seminfo s;

	switch (rid(n)) {
	case NT_OPT_EXPR:
		if (ralt_idx(n) == 0) /* <expr> */
			return bc_tw_eval_expr(v, rchild(n, 0));

		return 1;  /* ε */

	case NT_EXPR:
		switch (ralt_idx(n)) {
		case 0: {  /* ident <asgn_op> <add_expr> */
			struct rdesc_node asgn = rchild(n, 0);
			memcpy(&s, rseminfo(rchild(asgn, 0)), sizeof(s));

			/* <add_expr>'i left-associative yap */
			flip_if_not_flipped(asgn, 2);

			double rhs = bc_tw_eval_expr(v, rchild(asgn, 2));

			double *cur = map_get2(v, &s.ident_id, sizeof(size_t)), result;

			switch (ralt_idx(rchild(asgn, 1))) {
			case 0:  /* += */
				result = (cur ? *cur : 0) + rhs;
				break;

			case 1:  /* -= */
				result = (cur ? *cur : 0) - rhs;
				break;

			case 2:  /* *= */
				result = (cur ? *cur : 0) * rhs;
				break;

			case 3:  /* /= */
				result = (cur ? *cur : 0) / rhs;
				break;

			case 4:  /* %= */
				result = fmod(cur ? *cur : 0, rhs);
				break;

			case 5:  /* ^= */
				result = pow(cur ? *cur : 0, rhs);
				break;

			default: /* =  */
				result = rhs;

			}

			if (cur)
				*cur = result;
			else
				map_insert2(v, &s.ident_id, sizeof(size_t), &result);

			return result;
		}

		default:  /* <rel_expr> */
			return bc_tw_eval_expr(v, rchild(n, 0));
		}

	case NT_REL_EXPR: {  /* <add_expr> <rel_expr_opt_rel> */
		/* <add_expr>'i left-associative yap */
		flip_if_not_flipped(n, 0);
		double lhs = bc_tw_eval_expr(v, rchild(n, 0));

		struct rdesc_node opt = rchild(n, 1);

		if (ralt_idx(opt) == 1)
			return lhs;  /* ε */

		/* <add_expr>'i left-associative yap */
		flip_if_not_flipped(opt, 1);
		double rhs = bc_tw_eval_expr(v, rchild(opt, 1));

		switch (ralt_idx(rchild(opt, 0))) {
		case 0:
			return lhs < rhs;
		case 1:
			return lhs > rhs;
		case 2:
			return lhs == rhs;
		case 3:
			return lhs != rhs;
		case 4:
			return lhs <= rhs;
		case 5:
			return lhs >= rhs;
		default:
			assert(0);  // GCOVR_EXCL_LINE: unreachable
		}
	}

	case NT_ADD_EXPR: {
		double lhs;

		switch (ralt_idx(n)) {
		case 0: /* <add_expr> <add_op> <mul_expr> */
			lhs = bc_tw_eval_expr(v, rchild(n, 0));

			/* <mul_expr>'i left-associative yap */
			flip_if_not_flipped(n, 2);
			lhs += bc_tw_eval_expr(v, rchild(n, 2)) *
				(ralt_idx(rchild(n, 1)) == 0 ? 1 : -1);
			break;

		case 1:  /* <mul_expr> */
			/* <mul_expr>'i left-associative yap */
			flip_if_not_flipped(n, 0);

			lhs = bc_tw_eval_expr(v, rchild(n, 0));
			break;
		}

		return lhs;
	}

	case NT_MUL_EXPR: {
		double lhs = bc_tw_eval_expr(v, rchild(n, 0));
		double rhs;

		switch (ralt_idx(n)) {
		case 0: /* <mul_expr> <mul_op> <exp_expr> */
			rhs = bc_tw_eval_expr(v, rchild(n, 2));

			switch (ralt_idx(rchild(n, 1))) {
			case 0:
				lhs *= rhs;
				break;

			case 1:
				lhs /= rhs;
				break;

			case 2:
				lhs = fmod(lhs, rhs);
				break;
			}

			break;

		case 1:  /* <exp_expr> */
			break;
		}

		return lhs;
	}

	case NT_EXP_EXPR: {
		/* Üs alma right-associative'dir */
		double lhs = bc_tw_eval_expr(v, rchild(n, 0));
		double rhs = bc_tw_eval_expr(v, rchild(n, 1));

		return pow(lhs, rhs);
	}

	case NT_EXP_EXPR_REST: {
		double lhs, rhs;

		if (ralt_idx(n) == 0) {
			lhs = bc_tw_eval_expr(v, rchild(n, 1));
			rhs = bc_tw_eval_expr(v, rchild(n, 2));

			return pow(lhs, rhs);
		}

		return 1;
	}

	case NT_FACTOR: {  /* <unary_op> <atom> */
		double val = bc_tw_eval_expr(v, rchild(n, 1));

		return (ralt_idx(rchild(n, 0)) == 1) ? -val : val;
	}

	case NT_ATOM:
		switch (ralt_idx(n)) {
		case 0: {  /* <incr_decr_op> ident  (prefix) */
			memcpy(&s, rseminfo(rchild(n, 1)), sizeof(s));
			double *cur = map_get2(v, &s.ident_id, sizeof(size_t));

			double val = (cur ? *cur : 0) +
				(ralt_idx(rchild(n, 0)) == 0 ? 1 : -1);

			if (cur)
				*cur = val;
			else
				map_insert2(v, &s.ident_id, sizeof(size_t), &val);

			return val;
		}
		case 1: {  /* ident <opt_incr_decr_op>  (postfix veya plain) */
			memcpy(&s, rseminfo(rchild(n, 0)), sizeof(s));
			double *cur = map_get2(v, &s.ident_id, sizeof(size_t));

			double val = (cur ? *cur : 0);
			double old_val = val;

			switch (ralt_idx(rchild(n, 1))) {
			case 0:
				val++;
				break;
			case 1:
				val--;
				break;
			default:
				break;
			}

			if (cur)
				*cur = val;
			else
				map_insert2(v, &s.ident_id, sizeof(size_t), &val);

			return old_val;
		}

		case 2:  /* ( <expr> ) */
			return bc_tw_eval_expr(v, rchild(n, 1));

		case 3:  /* int */
			memcpy(&s, rseminfo(rchild(n, 0)), sizeof(s));

			return (double) s.num_int;

		case 4:  /* float */
			memcpy(&s, rseminfo(rchild(n, 0)), sizeof(s));

			return s.num_float;
		}
	}

	assert(0 && "Yakalanmamış expression."); // GCOVR_EXCL_LINE: unreachable
	return 0;
}
//! [bc_tw_eval_expr]
