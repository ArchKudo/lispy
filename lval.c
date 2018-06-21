#include "lval.h"
#include <errno.h>
#include <string.h>
#include "mpc.h"

LVal lval_wrap_num(long val) {
    LVal lval;
    lval.type = LVAL_NUM;
    lval.num = val;
    return lval;
}

LVal lval_wrap_err(int err) {
    LVal lval;
    lval.type = LVAL_ERR;
    lval.err = err;
    return lval;
}

LVal calc(char *op, LVal x, LVal y) {
    if (x.type == LVAL_ERR) {
        return x;
    }

    if (y.type == LVAL_ERR) {
        return y;
    }

    if (strcmp(op, "+") == 0) {
        return lval_wrap_num(x.num + y.num);
    } else if (strcmp(op, "-") == 0) {
        return lval_wrap_num(x.num - y.num);
    } else if (strcmp(op, "*") == 0) {
        return lval_wrap_num(x.num * y.num);
    } else if (strcmp(op, "/") == 0) {
        return y.num == 0 ? lval_wrap_err(LERR_DIV_ZERO)
                          : lval_wrap_num(x.num / y.num);
    } else if (strcmp(op, "%") == 0) {
        return lval_wrap_num(x.num % y.num);
    } else {
        return lval_wrap_err(LERR_BAD_OP);
    }
}

LVal eval(mpc_ast_t *node) {
    // Handle base case when leaf contains only a number
    if (strstr(node->tag, "num")) {
        errno = 0;
        // Convert contents to long with base 10
        long num = strtol(node->contents, NULL, 10);
        return errno != ERANGE ? lval_wrap_num(num)
                               : lval_wrap_err(LERR_BAD_NUM);
    }

    // Opeartor is always the first child
    char *op = node->children[1]->contents;

    // Evaluate the first expression
    // Setting res to 0, 1 will give wrong result for mul/add respectively.
    LVal res = eval(node->children[2]);

    // Recurse for later expressions
    int i = 3;
    while (strstr(node->children[i]->tag, "expr")) {
        res = calc(op, res, eval(node->children[i]));
        i++;
    }

    return res;
}

void print_lval(LVal lval) {
    switch (lval.type) {
        case LVAL_NUM:
            printf("%ld\n", lval.num);
            break;

        case LVAL_ERR:
            switch (lval.err) {
                case LERR_DIV_ZERO:
                    printf("Error cannot divide number with zero!\n");
                    break;
                case LERR_BAD_OP:
                    printf("Error invalid operator!\n");
                    break;
                case LERR_BAD_NUM:
                    printf("Error can't decide type of number!\n");
                    break;

                default:
                    printf("Unspecfied error!");
            }
            break;

        default:
            break;
    }
}
