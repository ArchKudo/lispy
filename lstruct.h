#ifndef LSTRUCT_H
#define LSTRUCT_H

struct LVal;
struct LEnv;

typedef struct LVal LVal;
typedef struct LEnv LEnv;

/* LVal Types */
enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_STR,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_FUN
};

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
    char *str;

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
 * @brief  A struct to store symbols with their (l)values
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

#endif
