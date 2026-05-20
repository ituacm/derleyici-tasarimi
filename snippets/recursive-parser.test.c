#include "../src/map.c"  // required-by: tokenizer.c

#include "lexer.c"
#include "recursive-parser.c"
#include "tokenizer.c"


int main()
{
	struct parser p;

	parser_init(&p);

//! [Çalışan örnek]
parser_eval(&p, "a = 2;"
		"print 2 + a * 3 + 1;");  // Ekrana 9 yazıyor!
//! [Çalışan örnek]

//! [Ters associativity]
parser_eval(&p, "print 10 - 2 - 3;");
//! [Ters associativity]

	parser_eval(&p, "a = 1;"
			"b = +2.3 + -3 / (4 * 2) * b + undefined;"
			"a = 3 - 2;"
			"print a * 2;"
			"print d;");


	parser_destroy(&p);
}
