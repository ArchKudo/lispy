#include "lprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lstruct.h"
#include "mpc.h"

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
        case LVAL_STR:
            lval_print_str(lval);
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

void lval_println(LVal *lval) {
    lval_print(lval);
    printf("\n");
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

void lval_print_str(LVal *lstr) {
    char *escaped = malloc(strlen(lstr->str) + 1);
    strcpy(escaped, lstr->str);
    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);
    free(escaped);
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
        case LVAL_STR:
            return "String";
        case LVAL_QEXPR:
            return "Quoted Expression";
        case LVAL_SEXPR:
            return "Symbolic Expression";
        default:
            return "Unknown type";
    }
}
