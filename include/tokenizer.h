/**
 * @file tokenizer.h
 * @brief @ref tokenizer_page
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H


#include "lexer.h"

#include "map.h"

#include <stddef.h>
#include <stdint.h>


/** @brief tokenizer. */
struct tokenizer {
	struct map ident_map;
	struct lexeme current_lexeme;

	struct map keywords;
	struct map punctuations;

	size_t last_id;
};

/** @brief Token. */
struct token {
	union {
		double num_float;
		intmax_t num_int;
		size_t ident_id;
	} seminfo;

	size_t tk_id;
};


/** Herhangi bir token bulunamadı. */
static const size_t TK_NOTOKEN = -1;

/** Tam sayı token ID'si. */
static const size_t TK_INT = -2;

/** Ondalık token ID'si. */
static const size_t TK_FLOAT = -3;

/** Identifier token ID'si. */
static const size_t TK_IDENT = -4;


/** @brief Yeni bir tokenizer oluşturur. */
void tokenizer_init(struct tokenizer *tokenizer);

/** @brief Tokenizerın tahsis ettiği belleği temizler. */
void tokenizer_destroy(struct tokenizer *tokenizer);

/** @brief Tokenizera bir keyword kaydeder. */
void tokenizer_add_keyword(struct tokenizer *tokenizer,
			   const char *keyword, size_t id);

/** @brief Tokenizera bir punctuation kaydeder.. */
void tokenizer_add_punctuation(struct tokenizer *tokenizer,
			       const char *punctuation, size_t id);

/** @brief Lexemeyi tokenizere gönderir. */
void tokenizer_feed(struct tokenizer *tokenizer, struct lexeme lexeme);

/** @brief Sıradaki tokeni çek. */
struct token tokenizer_next(struct tokenizer *tokenzier);


#endif
