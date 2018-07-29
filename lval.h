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
 * @brief  Evaluate an LVal
 * @note   Fetches symbols from LEnv, Handles SEXPR, or just returns LVal
 * @param  *lenv: LEnv from which the symbols must be fetched
 * @param  *lval: A LVal of any type
 * @retval A LVal computed depending on type (@see @note)
 */
LVal *lval_eval(LEnv *lenv, LVal *lval);

/**
 * @brief  Create a new LEnv
 * @retval A LEnv with fields initialized to NULL/0
 */
LEnv *lenv_new(void);

/**
 * @brief  Delete a LEnv
 * @param  *lenv: The LEnv to be deleted
 * @retval None
 */
void lenv_del(LEnv *lenv);

/**
 * @brief  Add default builtins to LEnv
 * @param  *lenv: The LEnv where the builtins are to be added
 * @retval None
 */
void lenv_init_builtins(LEnv *lenv);

#endif /* lval.h */
