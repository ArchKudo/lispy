#ifndef LVAL_H
#define LVAL_H

#include "mpc.h"

struct LVal;
struct LEnv;

typedef struct LVal LVal;
typedef struct LEnv LEnv;

/* LVal Types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

typedef LVal *(*LBuiltin)(LEnv *, LVal *);

/**
 * @brief  Store number or error in an abstract type
 * @note  LVal are lispy native values
 */
struct LVal {
    int type;
    long num;

    char *err;
    char *sym;
    LBuiltin fun;
    struct LVal **children;
    int child_count;
};

/**
 * @brief  A struct to store environment symbols and corresponding LVals as
 * arrays
 * @note   // Todo
 */
struct LEnv {
    char **syms;
    LVal **lvals;
    int child_count;
};

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
LVal *lval_eval(LEnv *lenv, LVal *lval);

#endif /* lval.h */
