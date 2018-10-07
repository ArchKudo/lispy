#include "lwrap.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "lenv.h"
#include "lstruct.h"

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

LVal *lval_wrap_str(char *str) {
    LVal *lstr = malloc(sizeof(LVal));
    lstr->type = LVAL_STR;
    lstr->str = malloc(strlen(str) + 1);
    strcpy(lstr->str, str);
    return lstr;
}
