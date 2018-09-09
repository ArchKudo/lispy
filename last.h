/* MPC AST handlers */

#ifndef LAST_H
#define LAST_H

#include "mpc.h"
#include "lstruct.h"

/**
 * @brief  Read ast as a LVal
 * @param  *node: The ast to be converted
 * @retval A LVal
 */
LVal *lval_read_ast(mpc_ast_t *node);

/**
 * @brief  Handler for lval_wrap_long
 * @note   Converts string to long converts to LVal on success
 * @param  *node: ast node containing a long number as contents
 * @retval A LVal of type LVAL_NUM on success, LVAL_ERR otherwise
 */
LVal *lval_read_long(mpc_ast_t *node);

/**
 * @brief  Handle unescaped wrapping of LVAL_STR
 * @param  *node: AST node having string as its contents
 * @retval A LVal of type
 */
LVal *lval_read_str(mpc_ast_t *node);

#endif
