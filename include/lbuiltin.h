/* LVal Builtins */

#ifndef LBUILTIN_H
#define LBUILTIN_H

#include "lstruct.h"
#include "lval.h"
#include "lwrap.h"
#include "lprint.h"

// Asserts condition `cond` is false, if not throws an error and deletes `lval`
#define LASSERT(lval, cond, fmt, ...)                                 \
    if (!(cond)) {                                                    \
        lval_del(lval);                                               \
        /* __VA_ARGS__ allows to variable number of arguments */      \
        /* ## in front eats "," on expansion if __VA_ARGS is empty */ \
        return lval_wrap_err(fmt, ##__VA_ARGS__);                     \
    }

// Asserts if child of `lval` at given index has the same type as `expected` or
// else, throw error
#define LASSERT_CHILD_TYPE(lbuiltin, lval, index, expected)                \
    LASSERT(lval, lval->children[index]->type == expected,                 \
            "Function '%s' was passed incorrect type of argument for "     \
            "argument: %i\n"                                               \
            "Got '%s' expected '%s'",                                      \
            lbuiltin, index, lval_print_type(lval->children[index]->type), \
            lval_print_type(expected))

// Asserts if correct number of arguments were passed, i.e by counting children
#define LASSERT_CHILD_COUNT(lbuiltin, lval, count)                     \
    LASSERT(lval, lval->child_count == count,                          \
            "Function '%s' was passed incorrect number of arguments\n" \
            "Got %i, expected %i",                                     \
            lbuiltin, lval->child_count, count)

// Asserts if function `lbuiltin` was passed with no arguments
#define LASSERT_CHILD_NOT_EMPTY(lbuiltin, lval, index)                         \
    LASSERT(lval, lval->children[index]->child_count != 0,                     \
            "Functions '%s' was passed {} for argument at index %i", lbuiltin, \
            index)

/**
 * @brief Convert a LVal sequence to qexpr
 * @param  *lval: A LVal
 * @param  *lenv: Not used
 * @retval Returns an LVal of type LVAL_QEXPR
 */
LVal *builtin_list(LEnv *lenv, LVal *lval);

/**
 * @brief  Returns head of a qexpr
 * @note   eg: head {0 1 2} => 0
 * @param  *lenv: Not used
 * @param  *lval: A LVal with type qexpr and atleast one child
 * @retval Head of qexpr
 */
LVal *builtin_head(LEnv *lenv, LVal *lval);

/**
 * @brief  Return the rest of the qexpr without the head
 * @note   eg: tail {0 1 2} => {1 2}
 * @param  *lenv: Not used
 * @param  *lval: A LVal with type qexpr and atleast one child
 * @retval Tail of qexpr
 */
LVal *builtin_tail(LEnv *lenv, LVal *lval);

/**
 * @brief  Evaluate a qexpr
 * @note   Evaluates a qexpr as a sexpr using lval_eval
 * @param  *lenv: Not used
 * @param  *lval: LVal of type LVAL_QEXPR
 * @retval Value of qexpr
 */
LVal *builtin_eval(LEnv *lenv, LVal *lval);

/**
 * @brief  Join multiple qexpr
 * @param  *lenv: Not used
 * @param  *lval: LVal of type LVAL_QEXPR with children of type LVAL_QEXPR
 * @retval The joined lval
 */
LVal *builtin_join(LEnv *lenv, LVal *lval);

/**
 * @brief  Evaluate operation LVAL_SYM between LVAL_NUM
 * @param  *lenv: Not used
 * @param  *lval: LVal containing numbers as children
 * @param  *op: Corresponding operator as string for the operation
 * @retval Result wrapped as LVal
 */
LVal *builtin_op(LEnv *lenv, LVal *lval, char *op);

/**
 * @brief  Evaluate comparision between two LVAL_NUM
 * @param  *lenv: Not used/Cast to void/Required for adding to a LEnv
 * @param  *lval: A LVal having children who are to be compared
 * @param  *op: The operator as a string
 * @retval Result wrapped as a LVal
 */
LVal *builtin_cmp(LEnv *lenv, LVal *lval, char *op);

/**
 * @brief  A builtin for if conditional
 * @param  *lenv: The corresponding lval
 * @param  *lval: The lval containing the condition and body
 * @retval Evaluation of lval which is in true condition
 */
LVal *builtin_if(LEnv *lenv, LVal *lval);

/**
 * @brief  Builtins for lambda expressions
 * @param  *lenv: The default LEnv
 * @param  *lval: LVal having two child QEXPR for lformals and lbody
 * @retval A LVal of type LVAL_FUN wrapped using lval_wrap_lambda
 */
LVal *builtin_lambda(LEnv *lenv, LVal *lval);

/**
 * @brief  Builtin for creating local, global functions
 * @note   Used by builtin_def, builtin_put
 * @param  *lenv: The LEnv to be operated on
 * @param  *lval: A LVal containing QEXPR for symbols(paramters) and, their
 * values
 * @param  *fun: Name of builtin, either "def" or "="
 * @retval Evaluated LVal
 */
LVal *builtin_var(LEnv *lenv, LVal *lval, char *fun);

/* A wrapper to builtin_var for global definitions */
LVal *builtin_def(LEnv *lenv, LVal *lval);

/* A wrapper to builtin_var for local definitions */
LVal *builtin_put(LEnv *lenv, LVal *lval);

/**
 * @brief  Load files containing valid lispy expression
 * @param  *lenv: The environment where the expressions are loaded
 * @param  *lval: The LVal containing the filename
 * @retval A LVal of result or LVAL_ERR on error
 */
LVal *builtin_load(LEnv *lenv, LVal *lval);

/**
 * @brief  Print a expression
 * @param  *lenv: A LEnv
 * @param  *lval: A LVal containing the expressions
 * @retval A LVal of type LVAL_SEXPR
 */
LVal *builtin_print(LEnv *lenv, LVal *lval);

/**
 * @brief  Create a LVAL_ERR from string
 * @param  *lenv: A LEnv
 * @param  *lval: LVal containing error string
 * @retval A LVal of type LVAL_ERR
 */
LVal *builtin_err(LEnv *lenv, LVal *lval);
/* Wrappers for single operations of builtin_op */
LVal *builtin_add(LEnv *lenv, LVal *lval);

LVal *builtin_sub(LEnv *lenv, LVal *lval);

LVal *builtin_mul(LEnv *lenv, LVal *lval);

LVal *builtin_div(LEnv *lenv, LVal *lval);

LVal *builtin_mod(LEnv *lenv, LVal *lval);

/* Wrappers for single operations of builtin_ord */
LVal *builtin_gt(LEnv *lenv, LVal *lval);

LVal *builtin_lt(LEnv *lenv, LVal *lval);

LVal *builtin_ge(LEnv *lenv, LVal *lval);

LVal *builtin_le(LEnv *lenv, LVal *lval);

/* Wrappers for single operations of builtin_cmp */
LVal *builtin_eq(LEnv *lenv, LVal *lval);

LVal *builtin_ne(LEnv *lenv, LVal *lval);

/**
 * @brief  Associate a builtin and its symbol in a LEnv
 * @param  *lenv: The LEnv where the builtin is to be stored
 * @param  *sym: The symbol name of the builtin
 * @param  lbuiltin: The lbuiltin to be associated with sym
 * @retval None
 */
void lenv_add_builtin(LEnv *lenv, char *sym, LBuiltin lbuiltin);

/**
 * @brief  Add default builtins to LEnv
 * @param  *lenv: The LEnv where the builtins are to be added
 * @retval None
 */
void lenv_init_builtins(LEnv *lenv);

#endif
