#ifndef LWRAP_H
#define LWRAP_H

/* LVal Wrapper functions */

#include "lstruct.h"

#define MAX_ERR 4096

/**
 * @brief  Convert a long to a LVal
 * @param  val: A long to be converted to a LVal
 * @retval A LVal with num field set and type LVAL_NUM
 */
LVal *lval_wrap_long(long val);

/**
 * @brief  Wrap a char string as a LVal symbol
 * @param  *str: String to wrap
 * @retval A LVal of type LVAL_SYM
 */
LVal *lval_wrap_sym(char *sym);

/**
 * @brief  Wrap error with variable arguments to LVal
 * @note Copies a maximum of MAX_ERR bytes including null,
 *       may lead to truncated error string
 * @param *fmt: Format of the given variable argument error string
 * @retval A LVal of type LVAL_ERR
 */
LVal *lval_wrap_err(char *fmt, ...);

/**
 * @brief  Wrap S/Q-Expressions
 * @param  type: Type of expression
 * @retval A LVal of type: `type`
 */
LVal *lval_wrap_expr(int type);

// lval_wrap_expr with type LVAL_SEXPR
LVal *lval_wrap_sexpr(void);

// lval_wrap_expr with type LVAL_QEXPR
LVal *lval_wrap_qexpr(void);

/**
 * @brief  Wrap a LBuiltin as an LVal
 * @param  lbuiltin: A LBuiltin
 * @retval A LVal with type LVAL_FUN
 */
LVal *lval_wrap_lbuiltin(LBuiltin lbuiltin);
/* LVal methods for modification and evaluation */

/**
 * @brief  Wrap as a lambda expression (local functions)
 * @note   // To-do: Correct explanation for @param
 * @param  *lformals: The formal part of the lambda expression
 * @param  *lbody: The body of the lambda expression
 * @retval A LVal with type LVAL_FUN with lbuiltin field set to NULL
 */
LVal *lval_wrap_lambda(LVal *lformals, LVal *lbody);

/**
 * @brief  Wrap a string as a LVal
 * @param  *str: The string to be wrapped
 * @retval A LVal of type LVAL_STR
 */
LVal *lval_wrap_str(char *str);

#endif
