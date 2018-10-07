/* Functions to operate on LVal */

#ifndef LVAL_H
#define LVAL_H

#include "lstruct.h"

/**
 * @brief  Delete a LVal
 * @param  *lval: The LVal which need to be freed along with its contents
 * @retval None
 */
void lval_del(LVal *lval);

/**
 * @brief  Add a LVal to another LVal
 * @param  *parent: The parent LVal, the child is added to this LVal
 * @param  *child: The child LVal which needs to be added
 * @retval Returns the parent LVal
 */
LVal *lval_add(LVal *parent, LVal *child);

/**
 * @brief  Add an LVal to the end of other
 * @param  *augend: The parent to which the other LVal is added
 * @param  *addend: The child which is joined with augend
 * @retval The joined LVal (augend += addend)
 */
LVal *lval_join(LVal *augend, LVal *addend);

/**
 * @brief  Pop child at ith position
 * @param  *lval: Parent LVal
 * @param  index: Location of child
 * @retval The popped child
 */
LVal *lval_pop(LVal *lval, int index);

/**
 * @brief  Get child at ith position, delete rest
 * @param  *lval: The parent lval
 * @param  index: Position of child
 * @retval Child at ith position
 */
LVal *lval_take(LVal *lval, int index);

/**
 * @brief  Call a function `lfun` with `largs` as arguments
 * @note   TODO: Expand on inner-workings
 * @param  *lenv: The LEnv where the lfun will be stored
 * @param  *lfun: A `lbuiltin` / local function
 * @param  *largs: The arguments to be passed to the function `lfun`
 * @retval A partial or complete evaluation of function on exhausting its
 * formals
 */
LVal *lval_call(LEnv *lenv, LVal *lfun, LVal *largs);

/**
 * @brief  Compare two LVal's
 * @param  *first: The first LVal
 * @param  *second: The second LVal
 * @retval 1 if they are equal, 0 otherwise
 */
int lval_eq(LVal *first, LVal *second);

/**
 * @brief  Evaluate an LVal
 * @note   Fetches symbols from LEnv, Handles SEXPR, or just returns LVal
 * @param  *lenv: LEnv from which the symbols must be fetched
 * @param  *lval: A LVal of any type
 * @retval A LVal computed depending on type (@see @note)
 */
LVal *lval_eval(LEnv *lenv, LVal *lval);

/**
 * @brief  Evaluate a S-Expression
 * @param  *lenv: A LEnv which contains symbol list
 * @param  *lval: A LVal of type LVAL_SEXPR
 * @retval Result wrapped as a LVal
 */
LVal *lval_eval_sexpr(LEnv *lenv, LVal *lval);

/**
 * @brief  Copy a LVal
 * @param  *lval: The LVal to be copied
 * @retval A LVal with same fields set as `lval`
 */
LVal *lval_copy(LVal *lval);

#endif /* lval.h */
