#include "lbuiltin.h"
#include "lstruct.h"
#include "mpc.h"
#include "parser.h"
#include "lval.h"
#include "lwrap.h"
#include "lprint.h"
#include "lenv.h"
#include "last.h"

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

LVal *builtin_load(LEnv *lenv, LVal *lval) {
    LASSERT_CHILD_COUNT("load", lval, 1);
    LASSERT_CHILD_TYPE("load", lval, 0, LVAL_STR);

    mpc_result_t result;

    if (mpc_parse_contents(lval->children[0]->str, Lisp, &result)) {
        LVal *lexpr = lval_read_ast(result.output);
        mpc_ast_delete(result.output);

        while (lexpr->child_count) {
            LVal *leval = lval_eval(lenv, lval_pop(lexpr, 0));
            if (leval->type == LVAL_ERR) {
                lval_println(leval);
            }
            lval_del(leval);
        }

        lval_del(lexpr);
        lval_del(lval);

        return lval_wrap_sexpr();
    } else {
        char *err = mpc_err_string(result.error);
        mpc_err_delete(result.error);

        LVal *lerr = lval_wrap_err("Could not load file: %s", err);
        free(err);
        lval_del(lval);

        return lerr;
    }
}

LVal *builtin_print(LEnv *lenv, LVal *lval) {
    (void)lenv;
    for (int i = 0; i < lval->child_count; i++) {
        lval_print(lval->children[i]);
        printf(" ");
    }
    printf("\n");
    lval_del(lval);

    return lval_wrap_sexpr();
}

LVal *builtin_err(LEnv *lenv, LVal *lval) {
    (void)lenv;
    LASSERT_CHILD_COUNT("error", lval, 1);
    LASSERT_CHILD_TYPE("error", lval, 0, LVAL_STR);

    LVal *lerr = lval_wrap_err(lval->children[0]->str);

    lval_del(lval);
    return lerr;
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

    lenv_add_builtin(lenv, "load", builtin_load);
    lenv_add_builtin(lenv, "print", builtin_print);
    lenv_add_builtin(lenv, "err", builtin_err);
}
