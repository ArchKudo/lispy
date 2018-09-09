#include "lenv.h"
#include <stdlib.h>
#include <string.h>
#include "lstruct.h"
#include "lval.h"
#include "lwrap.h"

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
