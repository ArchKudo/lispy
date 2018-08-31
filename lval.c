#include "lval.h"
#include <errno.h>
#include <string.h>
#include "mpc.h"

#define MAX_ERR 4096

// TODO: Add shorter error descriptions
// TODO: Prototype remaining builtins
// TODO: Refactor some fields and variable names
// TODO: Refactor consistent usage of lsym, lvar, ...
// TODO: Fix some descriptions of parameters in comments
// TODO: Split source file to multiple modular files

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

///////////////////////////////////////////////////////////////////////////////
/* Function Declarations */
///////////////////////////////////////////////////////////////////////////////

/* LVal Wrapper functions */

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

/* Functions to operate on LVal */

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

/* LEnv Functions */

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

/* MPC AST handlers */

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

/* LVal printing methods */

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
 * @brief  Print type of LVal
 * @param  type: Takes integer value of type for LVAL_TYPE enum
 * @retval String representing the LVAL_TYPE
 */
char *lval_print_type(int type);

/* LVal Builtins */

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

///////////////////////////////////////////////////////////////////////////////
/* Functions to wrap primitives as LVal */
///////////////////////////////////////////////////////////////////////////////

LVal *lval_wrap_long(long num) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_NUM;
    lval->num = num;
    return lval;
}

LVal *lval_wrap_sym(char *sym) {
    LVal *lsym = malloc(sizeof(LVal));
    lsym->type = LVAL_SYM;
    lsym->sym = malloc(strlen(sym) + 1);
    strcpy(lsym->sym, sym);
    return lsym;
}

LVal *lval_wrap_err(char *fmt, ...) {
    LVal *lerr = malloc(sizeof(LVal));
    lerr->type = LVAL_ERR;

    // Create a variable list to store the arguments
    va_list va;

    // Initialize the va_list
    // va_start takes two arguments:
    // The va_list and the last argument before ellipsis
    va_start(va, fmt);

    // Allocate maximum size of error string
    lerr->err = malloc(MAX_ERR);

    // Copy the va string to lerr->err atmost MAX_ERR bytes including null
    // Leaving space for null byte is therefore required
    vsnprintf(lerr->err, MAX_ERR - 1, fmt, va);

    // Resize(Reduce) lerr->err to size of va string from MAX_ERR
    // Again here, strlen doesn't output size of string including null byte
    lerr->err = realloc(lerr->err, strlen(lerr->err) + 1);

    // Free va_list
    va_end(va);

    return lerr;
}

LVal *lval_wrap_expr(int type) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = type;
    lval->child_count = 0;
    lval->children = NULL;
    return lval;
}

LVal *lval_wrap_sexpr(void) { return lval_wrap_expr(LVAL_SEXPR); }
LVal *lval_wrap_qexpr(void) { return lval_wrap_expr(LVAL_QEXPR); }

LVal *lval_wrap_lbuiltin(LBuiltin lbuiltin) {
    LVal *lfun = malloc(sizeof(LVal));
    lfun->type = LVAL_FUN;
    lfun->lbuiltin = lbuiltin;
    return lfun;
}

LVal *lval_wrap_lambda(LVal *lformals, LVal *lbody) {
    LVal *llambda = malloc(sizeof(LVal));
    llambda->type = LVAL_FUN;

    // lambdas are user functions so, lbuiltin field is set to NULL
    llambda->lbuiltin = NULL;

    // Also provide a new local environment
    llambda->lenv = lenv_new();

    // Set formals(parameter list) and the body of lambda
    llambda->lformals = lformals;
    llambda->lbody = lbody;

    return llambda;
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to operate on LVal struct */
///////////////////////////////////////////////////////////////////////////////

void lval_del(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
            break;
        case LVAL_FUN:
            if (!(lval->lbuiltin)) {
                lenv_del(lval->lenv);
                lval_del(lval->lformals);
                lval_del(lval->lbody);
            }
            break;
        case LVAL_ERR:
            free(lval->err);
            break;
        case LVAL_SYM:
            free(lval->sym);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < lval->child_count; i++) {
                lval_del(lval->children[i]);
            }
            free(lval->children);
            break;
    }
    free(lval);
}

LVal *lval_copy(LVal *lval) {
    LVal *copy = malloc(sizeof(LVal));
    copy->type = lval->type;
    switch (copy->type) {
        case LVAL_NUM:
            copy->num = lval->num;
            break;
        case LVAL_FUN:
            if (lval->lbuiltin) {
                copy->lbuiltin = lval->lbuiltin;
            } else {
                copy->lbuiltin = NULL;
                copy->lenv = lenv_copy(lval->lenv);
                copy->lformals = lval_copy(lval->lformals);
                copy->lbody = lval_copy(lval->lbody);
            }
            break;
        case LVAL_SYM:
            copy->sym = malloc(strlen(lval->sym) + 1);
            strcpy(copy->sym, lval->sym);
            break;
        case LVAL_ERR:
            copy->err = malloc(strlen(lval->err) + 1);
            strcpy(copy->err, lval->err);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            copy->child_count = lval->child_count;
            copy->children = malloc(sizeof(LVal) * copy->child_count);
            for (int i = 0; i < copy->child_count; i++) {
                copy->children[i] = lval_copy(lval->children[i]);
            }
            break;
    }
    return copy;
}

// Add a single child LVal to parent LVal
LVal *lval_add(LVal *parent, LVal *child) {
    parent->child_count += 1;
    parent->children =
        realloc(parent->children, sizeof(LVal *) * parent->child_count);

    // Last child is child_count - 1 due to 0-based indexing
    parent->children[parent->child_count - 1] = child;
    return parent;
}

// Add addend's children to augend
LVal *lval_join(LVal *augend, LVal *addend) {
    for (int i = 0; i < addend->child_count; i++) {
        augend = lval_add(augend, addend->children[i]);
    }
    free(addend->children);
    free(addend);
    return augend;
}

LVal *lval_pop(LVal *lval, int index) {
    LVal *popped = lval->children[index];

    // Move memory one LVal to the left
    memmove(&lval->children[index],      // Destination
            &lval->children[index + 1],  // Source
            (size_t)(sizeof(LVal *) * (lval->child_count - index - 1))  // Size
    );

    lval->child_count -= 1;

    // Resize the `lval->children` array to the new size (i.e child_count - 1)
    lval->children =
        realloc(lval->children, sizeof(LVal *) * lval->child_count);

    // Return LVal at the ith position
    return popped;
}

// TODO: What is the point of doing a memmove if the LVal is going to be
// deleted?
LVal *lval_take(LVal *lval, int index) {
    // Pop LVal at the given index
    LVal *popped = lval_pop(lval, index);

    // Delete the rest of the LVal
    lval_del(lval);

    // Return the popped value
    return popped;
}

LVal *lval_call(LEnv *lenv, LVal *lfun, LVal *largs) {
    // Return the lbuiltin itself if a lbuiltin
    if (lfun->lbuiltin) {
        return lfun->lbuiltin(lenv, largs);
    }

    // Store original args/params count
    int nargs = largs->child_count;
    int nparams = lfun->lformals->child_count;

    // Part I: Assignment of args to params
    while (largs->child_count) {
        // Handle when args are remaining even after parameters are exhausted
        if (lfun->lformals->child_count == 0) {
            lval_del(largs);
            return lval_wrap_err(
                "Function was passed too many arguments!\n"
                "Got %i, Expected %i",
                nargs, nparams);
        }

        // Get the first variable symbol
        LVal *lsym = lval_pop(lfun->lformals, 0);

        // Handle variable arguments
        if (strcmp(lsym->sym, "&") == 0) {
            // Check there is only a single param(formal) following "&"
            if (lfun->lformals->child_count != 1) {
                lval_del(largs);
                return lval_wrap_err(
                    "Function format invalid!\n"
                    "'&' not followed by a single symbol!");
            }

            // Get the next symbol
            LVal *lsym_next = lval_pop(lfun->lformals, 0);

            // Assign the next symbol the remaining args as qexpr
            lenv_put(lfun->lenv, lsym_next, builtin_list(lenv, largs));

            lval_del(lsym);
            lval_del(lsym_next);
            break;
        }
        // Else, without variable arguments
        // Get the first variable's value
        LVal *lsym_val = lval_pop(largs, 0);

        // Puts the symbol and its value inside the environment
        lenv_put(lfun->lenv, lsym, lsym_val);

        lval_del(lsym);
        lval_del(lsym_val);
    }

    // After exhausting all arguments delete `largs`
    lval_del(largs);

    // Part II: Evaluation

    if (lfun->lformals->child_count > 0 &&
        strcmp(lfun->lformals->children[0]->sym, "&") == 0) {
        // Handle edge-case when `&` is not followed by a single argument,
        // But wasn't caught by assignment due to absence of args
        if (lfun->lformals->child_count != 2) {
            return lval_wrap_err(
                "Function format invalid!\n"
                "'&' not followed by a single symbol!");
        }

        lval_del(lval_pop(lfun->lformals, 0));

        LVal *lsym_next = lval_pop(lfun->lformals, 0);
        LVal *lsym_val = lval_wrap_qexpr();

        lenv_put(lfun->lenv, lsym_next, lsym_val);

        lval_del(lsym_next);
        lval_del(lsym_val);
    }
    // If all params(formals) are bound, evaluate
    if (lfun->lformals->child_count == 0) {
        lfun->lenv->parent = lenv;
        return builtin_eval(
            lfun->lenv, lval_add(lval_wrap_sexpr(), lval_copy(lfun->lbody)));
    } else {
        // For partial evaluation, return a copy of function
        return lval_copy(lfun);
    }
}

int lval_eq(LVal *first, LVal *second) {
    if (first->type != second->type) {
        return 0;
    }

    int type = first->type;

    switch (type) {
        case LVAL_NUM:
            return (first->num == second->num);
        case LVAL_ERR:
            return (strcmp(first->err, second->err) == 0);
        case LVAL_SYM:
            return (strcmp(first->sym, second->sym) == 0);
        case LVAL_FUN:
            if (first->lbuiltin || second->lbuiltin) {
                return (first->lbuiltin == second->lbuiltin);
            } else {
                return (lval_eq(first->lformals, second->lformals) &&
                        lval_eq(first->lbody, second->lbody));
            }
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            if (first->child_count != second->child_count) {
                return 0;
            }
            for (int i = 0; i < first->child_count; i++) {
                if (!lval_eq(first->children[i], second->children[i])) {
                    return 0;
                }
            }
            return 1;
        default:
            return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to operate on LEnv's */
///////////////////////////////////////////////////////////////////////////////

LEnv *lenv_new(void) {
    LEnv *lenv = malloc(sizeof(LEnv));
    lenv->parent = NULL;
    lenv->lvals = NULL;
    lenv->syms = NULL;
    lenv->child_count = 0;

    return lenv;
}

void lenv_del(LEnv *lenv) {
    for (int i = 0; i < lenv->child_count; i++) {
        free(lenv->syms[i]);
        lval_del(lenv->lvals[i]);
    }

    free(lenv->lvals);
    free(lenv->syms);
    free(lenv);
}

LVal *lenv_get(LEnv *hay, LVal *pin) {
    // Check in local environment
    for (int i = 0; i < hay->child_count; i++) {
        if (strcmp(hay->syms[i], pin->sym) == 0) {
            return lval_copy(hay->lvals[i]);
        }
    }

    // If not found, check in its parent environment
    if (hay->parent) {
        return lenv_get(hay->parent, pin);
    }

    return lval_wrap_err("Unbound symbol: '%s'", pin->sym);
}

void lenv_put(LEnv *lenv, LVal *lsym, LVal *lval) {
    for (int i = 0; i < lenv->child_count; i++) {
        // Check if symbol already exists
        if (strcmp(lenv->syms[i], lsym->sym) == 0) {
            // If exists, delete it
            lval_del(lenv->lvals[i]);
            // Copy the new value from LVal
            lenv->lvals[i] = lval_copy(lval);
            return;
        }
    }

    // If symbol is not present
    // Increase child count
    lenv->child_count += 1;

    // Reallocate to accomodate new child
    lenv->lvals = realloc(lenv->lvals, sizeof(LVal *) * lenv->child_count);
    lenv->syms = realloc(lenv->syms, sizeof(char *) * lenv->child_count);

    // Set symbol and its value as last child
    lenv->syms[lenv->child_count - 1] = malloc(strlen(lsym->sym) + 1);
    strcpy(lenv->syms[lenv->child_count - 1], lsym->sym);

    lenv->lvals[lenv->child_count - 1] = lval_copy(lval);
}

LEnv *lenv_copy(LEnv *lenv) {
    LEnv *copy = malloc(sizeof(LEnv));
    copy->parent = lenv->parent;
    copy->child_count = lenv->child_count;
    copy->lvals = malloc(sizeof(LVal) * copy->child_count);
    copy->syms = malloc(sizeof(char *) * copy->child_count);

    for (int i = 0; i < copy->child_count; i++) {
        copy->syms[i] = malloc(strlen(lenv->syms[i]) + 1);
        strcpy(copy->syms[i], lenv->syms[i]);
        copy->lvals[i] = lval_copy(lenv->lvals[i]);
    }

    return copy;
}

void lenv_put_global(LEnv *lenv, LVal *lsym, LVal *lval) {
    while (lenv->parent) {
        lenv = lenv->parent;
    }
    lenv_put(lenv, lsym, lval);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LVal *lval_read_long(mpc_ast_t *node) {
    errno = 0;
    long val = strtol(node->contents, NULL, 10);
    return errno != ERANGE ? lval_wrap_long(val)
                           : lval_wrap_err("Number too large!");
}

LVal *lval_read_ast(mpc_ast_t *node) {
    if (strstr(node->tag, "num")) {
        return lval_read_long(node);
    }
    if (strstr(node->tag, "sym")) {
        return lval_wrap_sym(node->contents);
    }

    LVal *root = NULL;
    if ((strcmp(node->tag, ">") == 0) || (strstr(node->tag, "sexpr"))) {
        root = lval_wrap_sexpr();
    } else if (strstr(node->tag, "qexpr")) {
        root = lval_wrap_qexpr();
    }

    for (int i = 0; i < node->children_num; i++) {
        if ((strcmp(node->children[i]->contents, "(") == 0) ||
            (strcmp(node->children[i]->contents, ")") == 0) ||
            (strcmp(node->children[i]->contents, "{") == 0) ||
            (strcmp(node->children[i]->contents, "}") == 0) ||
            (strcmp(node->children[i]->tag, "regex") == 0)) {
            continue;
        } else {
            root = lval_add(root, lval_read_ast(node->children[i]));
        }
    }
    return root;
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to print LVal's */
///////////////////////////////////////////////////////////////////////////////

void lval_print(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
            printf("%ld", lval->num);
            break;
        case LVAL_FUN:
            if (lval->lbuiltin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(lval->lformals);
                printf(" ");
                lval_print(lval->lbody);
                printf(")");
            }
            break;
        case LVAL_ERR:
            printf("Error: %s", lval->err);
            break;
        case LVAL_SYM:
            printf("%s", lval->sym);
            break;
        case LVAL_SEXPR:
            lval_print_sexpr(lval);
            break;
        case LVAL_QEXPR:
            lval_print_qexpr(lval);
            break;
        default:
            break;
    }
}

void lval_print_expr(LVal *lval, char open, char close) {
    printf("%c", open);
    for (int i = 0; i < lval->child_count; i++) {
        lval_print(lval->children[i]);

        if (i != (lval->child_count - 1)) {
            printf(" ");
        }
    }
    printf("%c", close);
}

void lval_print_sexpr(LVal *lval) { lval_print_expr(lval, '(', ')'); }

void lval_print_qexpr(LVal *lval) { lval_print_expr(lval, '{', '}'); }

void lval_println(LVal *lval) {
    lval_print(lval);
    printf("\n");
}

char *lval_print_type(int type) {
    switch (type) {
        case LVAL_NUM:
            return "Number";
        case LVAL_FUN:
            return "Function";
        case LVAL_SYM:
            return "Symbol";
        case LVAL_ERR:
            return "Error";
        case LVAL_QEXPR:
            return "Quoted Expression";
        case LVAL_SEXPR:
            return "Symbolic Expression";
        default:
            return "Unknown type";
    }
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to evaluate LVal */
///////////////////////////////////////////////////////////////////////////////

LVal *lval_eval_sexpr(LEnv *lenv, LVal *lval) {
    // Start evaluation from the inner-most child
    for (int i = 0; i < lval->child_count; i++) {
        lval->children[i] = lval_eval(lenv, lval->children[i]);
    }

    // If there is an error return error, discard LVal
    for (int i = 0; i < lval->child_count; i++) {
        if (lval->children[i]->type == LVAL_ERR) {
            return lval_take(lval, i);
        }
    }

    // If no child return the LVal
    if (lval->child_count == 0) {
        return lval;
    }

    // For a single child pop child, delete LVal, return child
    if (lval->child_count == 1) {
        return lval_take(lval, 0);
    }

    // Get the first child
    LVal *lfun = lval_pop(lval, 0);

    // Raise error if first child is not a function
    // Also free first child and `lval`
    if (lfun->type != LVAL_FUN) {
        LVal *lerr = lval_wrap_err(
            "S-Expression starts with incorrect type!\n"
            "Got %s, Expected %s",
            lval_print_type(lfun->type), lval_print_type(LVAL_FUN));
        lval_del(lfun);
        lval_del(lval);
        return lerr;
    }

    // Invoke function
    LVal *result = lval_call(lenv, lfun, lval);

    // Delete the first child
    lval_del(lfun);

    return result;
}

LVal *lval_eval(LEnv *lenv, LVal *lval) {
    // If a symbol get value from LEnv
    if (lval->type == LVAL_SYM) {
        LVal *lsym = lenv_get(lenv, lval);
        lval_del(lval);
        return lsym;
    }

    if (lval->type == LVAL_SEXPR) {
        return lval_eval_sexpr(lenv, lval);
    }

    return lval;
}

///////////////////////////////////////////////////////////////////////////////
/* Language built-in(LEnv) functions for operation on different LVal types */
///////////////////////////////////////////////////////////////////////////////

LVal *builtin_list(LEnv *lenv, LVal *lval) {
    (void)lenv;
    lval->type = LVAL_QEXPR;
    return lval;
}

LVal *builtin_head(LEnv *lenv, LVal *lval) {
    (void)lenv;
    // Assert head is passed a single argument
    // i.e head has a single child
    LASSERT_CHILD_COUNT("head", lval, 1);

    // Assert head's first child is a QEXPR
    LASSERT_CHILD_TYPE("head", lval, 0, LVAL_QEXPR);

    // Assert QEXPR passed to head was not empty
    LASSERT_CHILD_NOT_EMPTY("head", lval, 0);

    // Get first child i.lenv first argument of head
    LVal *qexpr = lval_take(lval, 0);

    // Pop all elements(children) of qexpr except first
    while (qexpr->child_count > 1) {
        lval_del(lval_pop(qexpr, 1));
    }

    return qexpr;
}

LVal *builtin_tail(LEnv *lenv, LVal *lval) {
    (void)lenv;
    LASSERT_CHILD_COUNT("tail", lval, 1);

    LASSERT_CHILD_TYPE("tail", lval, 0, LVAL_QEXPR);

    LASSERT_CHILD_NOT_EMPTY("tail", lval, 0);

    // Get heads first argument
    LVal *qexpr = lval_take(lval, 0);

    // Only delete the first child of the argument
    lval_del(lval_pop(qexpr, 0));
    return qexpr;
}

LVal *builtin_eval(LEnv *lenv, LVal *lval) {
    (void)lenv;
    LASSERT_CHILD_COUNT("eval", lval, 1);

    LASSERT_CHILD_TYPE("eval", lval, 0, LVAL_QEXPR);

    // Get the first argument
    LVal *qexpr = lval_take(lval, 0);
    // Evaluate as an SEXPR
    qexpr->type = LVAL_SEXPR;
    return lval_eval(lenv, qexpr);
}

LVal *builtin_join(LEnv *lenv, LVal *lval) {
    (void)lenv;
    for (int i = 0; i < lval->child_count; i++) {
        LASSERT_CHILD_TYPE("join", lval, i, LVAL_QEXPR);
    }

    // Get the first arg of head
    LVal *qexpr = lval_pop(lval, 0);

    // Join the remaining args to the first
    while (lval->child_count) {
        qexpr = lval_join(qexpr, lval_pop(lval, 0));
    }

    // Delete the LVal
    lval_del(lval);
    return qexpr;
}

LVal *builtin_op(LEnv *lenv, LVal *lval, char *op) {
    (void)lenv;
    // Error handling for non-number values
    for (int i = 0; i < lval->child_count; i++) {
        LASSERT_CHILD_TYPE(op, lval, i, LVAL_NUM);
    }

    // Get the first operand
    // Side-Effect lval->child_count--;
    LVal *first = lval_pop(lval, 0);

    // If only first operand is supplied with `-` operator negate first operand
    if ((strcmp(op, "-") == 0) && lval->child_count == 0) {
        first->num = -first->num;
    }

    while (lval->child_count > 0) {
        // Get the other(second) operand
        LVal *second = lval_pop(lval, 0);

        if (strcmp(op, "+") == 0) {
            first->num += second->num;
        }

        if (strcmp(op, "-") == 0) {
            first->num -= second->num;
        }

        if (strcmp(op, "*") == 0) {
            first->num *= second->num;
        }

        if (strcmp(op, "/") == 0) {
            if (second->num == 0) {
                // Free first and second
                lval_del(first);
                lval_del(second);

                first = lval_wrap_err("Cannot divide by zero!");
                break;
            }

            first->num /= second->num;
        }

        if (strcmp(op, "%") == 0) {
            first->num %= second->num;
        }

        lval_del(second);
    }

    lval_del(lval);
    return first;
}

LVal *builtin_mod(LEnv *lenv, LVal *lval) {
    return builtin_op(lenv, lval, "%");
}

LVal *builtin_ord(LEnv *lenv, LVal *lval, char *op) {
    (void)lenv;

    LASSERT_CHILD_COUNT(op, lval, 2);
    LASSERT_CHILD_TYPE(op, lval, 0, LVAL_NUM);
    LASSERT_CHILD_TYPE(op, lval, 1, LVAL_NUM);

    long result = 0;
    long first = lval->children[0]->num;
    long second = lval->children[1]->num;

    if (strcmp(op, "<") == 0) {
        result = (first < second);
    } else if (strcmp(op, ">") == 0) {
        result = (first > second);
    } else if (strcmp(op, "<=") == 0) {
        result = (first <= second);
    } else if (strcmp(op, ">=") == 0) {
        result = (first >= second);
    }

    lval_del(lval);
    return lval_wrap_long(result);
}

LVal *builtin_cmp(LEnv *lenv, LVal *lval, char *op) {
    (void)lenv;

    LASSERT_CHILD_COUNT(op, lval, 2);

    int result = 0;
    LVal *first = lval->children[0];
    LVal *second = lval->children[1];

    if (strcmp(op, "==") == 0) {
        result = lval_eq(first, second);
    } else if (strcmp(op, "!=") == 0) {
        result = !lval_eq(first, second);
    }

    lval_del(lval);
    return lval_wrap_long(result);
}

LVal *builtin_if(LEnv *lenv, LVal *lval) {
    LASSERT_CHILD_COUNT("if", lval, 3);

    LASSERT_CHILD_TYPE("if", lval, 0, LVAL_NUM);
    LASSERT_CHILD_TYPE("if", lval, 1, LVAL_QEXPR);
    LASSERT_CHILD_TYPE("if", lval, 2, LVAL_QEXPR);

    LVal *result;

    // Convert expressions to be evaluable
    lval->children[1]->type = LVAL_SEXPR;
    lval->children[2]->type = LVAL_SEXPR;

    if (lval->children[0]->num) {
        result = lval_eval(lenv, lval_pop(lval, 1));
    } else {
        result = lval_eval(lenv, lval_pop(lval, 2));
    }

    lval_del(lval);

    return result;
}

LVal *builtin_var(LEnv *lenv, LVal *lval, char *fun) {
    // Syntax is of the form:
    //`fun {sym1, sym2, ...} val1, val2, ...`
    //     ^- Child 0        ^- Child 1  ^- Child N

    // Check whether Child 0 is a QEXPR
    LASSERT_CHILD_TYPE(fun, lval, 0, LVAL_QEXPR);

    // Get the symbol list(QEXPR)
    LVal *var_list = lval->children[0];

    // Assert all var_list members are symbols
    for (int i = 0; i < var_list->child_count; i++) {
        LASSERT_CHILD_TYPE(fun, var_list, i, LVAL_SYM);
    }

    // Assert there are enough values for all symbols in var list
    LASSERT_CHILD_COUNT(fun, var_list, lval->child_count - 1);

    for (int i = 0; i < var_list->child_count; i++) {
        // If fun is "def" put in global scope i.e parent environment
        if (strcmp(fun, "def") == 0) {
            lenv_put_global(lenv, var_list->children[i], lval->children[i + 1]);
        } else if (strcmp(fun, "=") == 0) {  // Else in local environment
            // Put variable symbols and their values in the env
            lenv_put(lenv, var_list->children[i], lval->children[i + 1]);
        }
    }

    lval_del(lval);
    return lval_wrap_sexpr();
}

LVal *builtin_lambda(LEnv *lenv, LVal *lval) {
    (void)lenv;
    // Ensure both lformals and lbody are present
    LASSERT_CHILD_COUNT("\\", lval, 2);

    // Assert both children are QEXPR
    LASSERT_CHILD_TYPE("\\", lval, 0, LVAL_QEXPR);
    LASSERT_CHILD_TYPE("\\", lval, 1, LVAL_QEXPR);

    // Check if first child(lformals) contains only symbols
    for (int i = 0; i < lval->children[0]->child_count; i++) {
        LASSERT(lval, (lval->children[0]->children[i]->type == LVAL_SYM),
                "Formals can only contain symbols!\n"
                "Got %s, Expected %s",
                lval_print_type(lval->children[0]->children[i]->type),
                lval_print_type(LVAL_SYM));
    }

    LVal *lformals = lval_pop(lval, 0);
    LVal *lbody = lval_pop(lval, 0);

    lval_del(lval);

    return lval_wrap_lambda(lformals, lbody);
}

LVal *builtin_def(LEnv *lenv, LVal *lval) {
    return builtin_var(lenv, lval, "def");
}

LVal *builtin_put(LEnv *lenv, LVal *lval) {
    return builtin_var(lenv, lval, "=");
}

LVal *builtin_add(LEnv *lenv, LVal *lval) {
    return builtin_op(lenv, lval, "+");
}

LVal *builtin_sub(LEnv *lenv, LVal *lval) {
    return builtin_op(lenv, lval, "-");
}

LVal *builtin_mul(LEnv *lenv, LVal *lval) {
    return builtin_op(lenv, lval, "*");
}

LVal *builtin_div(LEnv *lenv, LVal *lval) {
    return builtin_op(lenv, lval, "/");
}

LVal *builtin_gt(LEnv *lenv, LVal *lval) {
    return builtin_ord(lenv, lval, ">");
}
LVal *builtin_lt(LEnv *lenv, LVal *lval) {
    return builtin_ord(lenv, lval, "<");
}
LVal *builtin_ge(LEnv *lenv, LVal *lval) {
    return builtin_ord(lenv, lval, ">=");
}
LVal *builtin_le(LEnv *lenv, LVal *lval) {
    return builtin_ord(lenv, lval, "<=");
}

LVal *builtin_eq(LEnv *lenv, LVal *lval) {
    return builtin_cmp(lenv, lval, "==");
}

LVal *builtin_ne(LEnv *lenv, LVal *lval) {
    return builtin_cmp(lenv, lval, "!=");
}

void lenv_add_builtin(LEnv *lenv, char *sym, LBuiltin lbuiltin) {
    LVal *lsym = lval_wrap_sym(sym);
    LVal *lfun = lval_wrap_lbuiltin(lbuiltin);

    lenv_put(lenv, lsym, lfun);
    lval_del(lsym);
    lval_del(lfun);
}

void lenv_init_builtins(LEnv *lenv) {
    lenv_add_builtin(lenv, "list", builtin_list);
    lenv_add_builtin(lenv, "head", builtin_head);
    lenv_add_builtin(lenv, "tail", builtin_tail);
    lenv_add_builtin(lenv, "eval", builtin_eval);
    lenv_add_builtin(lenv, "join", builtin_join);

    lenv_add_builtin(lenv, "\\", builtin_lambda);
    lenv_add_builtin(lenv, "def", builtin_def);

    lenv_add_builtin(lenv, "+", builtin_add);
    lenv_add_builtin(lenv, "-", builtin_sub);
    lenv_add_builtin(lenv, "*", builtin_mul);
    lenv_add_builtin(lenv, "/", builtin_div);
    lenv_add_builtin(lenv, "%", builtin_mod);
    lenv_add_builtin(lenv, "=", builtin_put);

    lenv_add_builtin(lenv, "if", builtin_if);
    lenv_add_builtin(lenv, "==", builtin_eq);
    lenv_add_builtin(lenv, "!=", builtin_ne);
    lenv_add_builtin(lenv, ">", builtin_gt);
    lenv_add_builtin(lenv, "<", builtin_lt);
    lenv_add_builtin(lenv, ">=", builtin_ge);
    lenv_add_builtin(lenv, "<=", builtin_le);
}

///////////////////////////////////////////////////////////////////////////////
/* LVAL_C */
///////////////////////////////////////////////////////////////////////////////
