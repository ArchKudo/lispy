/* Functions to operate on LEnv */

#ifndef LENV_H
#define LENV_H

#include "lstruct.h"

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
 * @brief  Search for LVal of type LVAL_SYM in LEnv "hay" containing the same
 * symbol(sym) as "pin"
 * @param  *hay: LEnv "hay" to search the symbol(LVal) "pin" in
 * @param  *pin: The symbol(LVal) "pin" to be searched for in the LEnv "hay"
 * @retval A copy of LVal in "hay" having symbol same as "pin" if exists, else
 * error of the type LVAL_ERR
 */
LVal *lenv_get(LEnv *hay, LVal *pin);

/**
 * @brief  Put an LVal with symbol lsym inside a LEnv
 * @param  *lenv: A LEnv in which the LVal is to be added
 * @param  *lsym: The symbol of the LVal which is to be added into LEnv
 * @param  *lval: The LVal with symbol lsym which is to be added into LEnv
 * @retval None
 */
void lenv_put(LEnv *lenv, LVal *lsym, LVal *lval);

/**
 * @brief  Copy a LEnv
 * @param  *lenv: The LEnv to be copied
 * @retval The copied LEnv
 */
LEnv *lenv_copy(LEnv *lenv);

/**
 * @brief  Put a symbol inside a global environment
 * @param  *lenv: Immediate LEnv environment
 * @param  *lsym: The symbol used to represent lval, of type LVAL_SYM
 * @param  *lval: The value of the symbol
 * @retval None
 */
void lenv_put_global(LEnv *lenv, LVal *lsym, LVal *lval);

#endif
