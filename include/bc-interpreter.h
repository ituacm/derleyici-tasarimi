/**
 * @file bc-interpreter.h
 * @brief @ref bc_interpreter_page
 */

#ifndef BC_INTERPRETER_H
#define BC_INTERPRETER_H


#include "map.h"

#include <rdesc/rdesc.h>


/** @brief Interpreter. */
void bc_cli();

/** @brief bc statement interpret eder. */
void bc_interpret(struct map *v, struct rdesc_node n);


#endif
