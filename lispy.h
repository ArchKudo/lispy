#include "mpc.h"
#include "lstruct.h"
#include "last.h"
#include "lval.h"
#include "lenv.h"
#include "lbuiltin.h"

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

/**
 * @brief  Add a LVal to another LVal
 * @param  *parent: The parent LVal, the child is added to this LVal
 * @param  *child: The child LVal which needs to be added
 * @retval Returns the parent LVal
 */
LVal *lval_add(LVal *parent, LVal *child);

/**
 * @brief  Create a empty S-Expression
 * @note   Wrapper for lval_wrap_expr
 * @retval A LVal with type LVAL_SEXPR
 */
LVal *lval_wrap_sexpr(void);

/**
 * @brief  Wrap a string as a LVal
 * @param  *str: The string to be wrapped
 * @retval A LVal of type LVAL_STR
 */
LVal *lval_wrap_str(char *str);

/**
 * @brief  Load files containing valid lispy expression
 * @param  *lenv: The environment where the expressions are loaded
 * @param  *lval: The LVal containing the filename
 * @retval A LVal of result or LVAL_ERR on error
 */
LVal *builtin_load(LEnv *lenv, LVal *lval);
