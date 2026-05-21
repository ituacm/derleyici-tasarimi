/**
 * @file bc-interpreter.h
 * @brief @ref bc_interpreter_page
 */

#ifndef BC_INTERPRETER_H
#define BC_INTERPRETER_H


#include "map.h"

#include <rdesc/rdesc.h>

#include <stdbool.h>


/** @brief Statement'lerden dönülen ekstra bilgi. */
enum stmt_meta {
	VOID,
	BREAK,
	QUIT,
};


/** @brief Interpreter. */
void bc_cli();

/** @brief Bir bc statement'ini interpret eder. */
enum stmt_meta bc_tw_eval_stmt(struct map *v, struct rdesc_node n);

/** @brief Bir bc expression'unu interpret eder. */
double bc_tw_eval_expr(struct map *v, struct rdesc_node n);


#endif
