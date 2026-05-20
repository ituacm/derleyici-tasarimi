#include "../include/bc-grammar.h"

#include <assert.h>

#include <rdesc/grammar.h>
#include <rdesc/util.h>

#include "bc-grammar.c"


int main()
{
	struct rdesc_grammar grammar;

	assert(rdesc_grammar_init_checked(&grammar,
					  BC_NT_COUNT,
					  BC_MAX_ALT_COUNT,
					  BC_MAX_ALT_SIZE,
					  bc_production_rules) == 0);

//! [Dump grammar]
rdesc_dump_bnf(stdout, &grammar, bc_tk_names, bc_nt_names);
//! [Dump grammar]

	rdesc_grammar_destroy(&grammar);
}
