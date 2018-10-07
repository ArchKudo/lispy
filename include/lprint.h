/* LVal printing methods */

#ifndef LPRINT_H
#define LPRINT_H

#include "lstruct.h"


/**
 * @brief Print value of LVal adding a new line at the end
 * @param  val: An LVal
 * @retval None
 */
void lval_println(LVal *lval);

/**
 * @brief Print value of LVal
 * @note Switches between different types of LVal and prints appropriately
 * @param  val: An LVal
 * @retval None
 */
void lval_print(LVal *lval);

/**
 * @brief Print an S-Expression
 * @note lval_print handler for S-Expression
 * @param  *lval: LVal with type LVAL_SEXPR
 * @param  open: Character to be inserted before S-Expression
 * @param  close: Character to be inserted at end of S-Expression
 * @retval None
 */
void lval_print_expr(LVal *lval, char open, char close);

// lval_print_expr wrapper for S-Expressions
void lval_print_sexpr(LVal *lval);
// lval_print_expr wrapper for Q-Expressions
void lval_print_qexpr(LVal *lval);

/**
 * @brief  Print a escaped string
 * @param  *lstr: A LVal of type LVAL_STR
 * @retval None
 */
void lval_print_str(LVal *lstr);

/**
 * @brief  Print type of LVal
 * @param  type: Takes integer value of type for LVAL_TYPE enum
 * @retval String representing the LVAL_TYPE
 */
char *lval_print_type(int type);

#endif
