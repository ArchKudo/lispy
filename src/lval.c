#include "lval.h"
#include <stdlib.h>
#include <string.h>
#include "lstruct.h"
#include "lenv.h"
#include "lwrap.h"
#include "lbuiltin.h"
#include "lprint.h"

// TODO: Add shorter error descriptions
// TODO: Refactor some fields and variable names
// TODO: Refactor consistent usage of lsym, lvar, ...
// TODO: Fix some descriptions of parameters in comments

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
        case LVAL_STR:
            free(lval->str);
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
        case LVAL_STR:
            copy->str = malloc(strlen(lval->str) + 1);
            strcpy(copy->str, lval->str);
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
    memmove(&lval->children[index],                             // Destination
            &lval->children[index + 1],                         // Source
            (sizeof(LVal *) * (lval->child_count - index - 1))  // Size
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
        case LVAL_STR:
            return (strcmp(first->str, second->str) == 0);
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
