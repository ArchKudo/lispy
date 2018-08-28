#ifndef LVAL_H
#define LVAL_H

#include "mpc.h"

struct LVal;
struct LEnv;

typedef struct LVal LVal;
typedef struct LEnv LEnv;

/* LVal Types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

/**
 * @brief  A function pointer for LBuiltins
 * @note   LBuiltins are lispy's native functions
 * @param  LEnv *: A LEnv to hold environment variables
 * @param  LVal *: A list of LVals to operate on
 * @retval Evaluation as a LVal
 */
typedef LVal *(*LBuiltin)(LEnv *, LVal *);

/**
 * @brief  Store number or error in an abstract type
 * @note  LVal are lispy native values
 */
struct LVal {
    /* Type */
    int type;

    /* Value */
    long num;
    char *err;
    char *sym;

    /* Functions */
    LBuiltin lbuiltin;
    LEnv *lenv;
    LVal *lformals;
    LVal *lbody;

    /* Child Expressions */
    struct LVal **children;
    int child_count;
};

/**
 * @brief  A struct to store variables with their (l)values
 * @note   LEnv maybe local to function or the parent environment
 */
struct LEnv {
    /* Points to the parent environment */
    LEnv *parent;

    /* List of symbols in the environment */
    char **syms;
    /* And their corresponding LVals */
    LVal **lvals;

    int child_count;
};

/**
 * @brief Print the value of a LVal, adding a new line at the end
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
