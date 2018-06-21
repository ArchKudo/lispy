#ifndef LVAL_H
#define LVAL_H

#include "mpc.h"

/* Define some errors */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* LVal Types */
enum { LVAL_NUM, LVAL_ERR };

/**
 * @brief  Store number or error in an abstract type
 * @note  LVal are lispy native values
 */
typedef struct {
    int type;
    int err;
    long num;
} LVal;

/**
 * @brief Print value of LVal
 * @param  val: An LVal
 * @retval None
 */
void print_lval(LVal val);

/**
 * @brief  Evaluates RPN ast recursively
 * @param  *node: Parent/Root node of tree
 * @retval Result of expression
 */
LVal eval(mpc_ast_t *node);

#endif /* lval.h */
