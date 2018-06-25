#ifndef LVAL_H
#define LVAL_H

#include "mpc.h"

/* Define some errors */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* LVal Types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

/**
 * @brief  Store number or error in an abstract type
 * @note  LVal are lispy native values
 */
typedef struct LVal {
    int type;
    long num;

    char *err;
    char *sym;
    struct LVal **children;
    int children_count;
} LVal;

/**
 * @brief Print value of LVal adding a new line at the end
 * @param  val: An LVal
 * @retval None
 */
void lval_println(LVal *lval);

/**
 * @brief  Read ast as a LVal
 * @param  *node: The ast to be converted
 * @retval A LVal
 */
LVal *lval_read_ast(mpc_ast_t *node);

/**
 * @brief  Delete a LVal
 * @param  *lval: The LVal which need to be freed along with its contents
 * @retval None
 */
void lval_del(LVal *lval);

/**
 * @brief  Evaluates RPN ast recursively
 * @param  *node: Parent/Root node of tree
 * @retval Result of expression
 */
// LVal eval(mpc_ast_t *node);

#endif /* lval.h */
