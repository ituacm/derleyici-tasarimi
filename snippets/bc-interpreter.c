#include "../include/bc-grammar.h"
#include "../include/bc-interpreter.h"
#include "../include/tokenizer.h"

#include "../include/map.h"

#include <rdesc/grammar.h>
#include <rdesc/rdesc.h>

#include <assert.h>
#include <stdio.h>


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

	while ((fgets(buf, 4096, stdin)) != NULL) {
		lexer_init(&l, buf);

		struct token tk;

		do {
			/* Sıradaki token'i tk içine çeker. */
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

			if (tk.id != TK_NOTOKEN) {
				switch (rdesc_pump(&p, tk.id, &tk.seminfo)) {
				case RDESC_READY:
					bc_interpret(&v, rdesc_get_root(&p));

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
		} while (tk.id != TK_NOTOKEN);
	}

	tokenizer_destroy(&t);
	rdesc_destroy(&p);
	rdesc_grammar_destroy(&g);
	map_destroy(&v);
}
//! [bc_cli]

//! [bc_interpret]
void bc_interpret(struct map *v, struct rdesc_node n)
{
	(void) v; (void) n;
}
//! [bc_interpret]
