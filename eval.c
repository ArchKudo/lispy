#include "eval.h"
#include <string.h>
#include "mpc.h"

long calc(char *op, long x, long y) {
    // TODO: Research is this the only way to do this..
    if (strcmp(op, "+") == 0) {
        return x + y;
    } else if (strcmp(op, "-") == 0) {
        return x - y;
    } else if (strcmp(op, "*") == 0) {
        return x * y;
    } else if (strcmp(op, "/") == 0) {
        return x / y;
    } else if (strcmp(op, "%") == 0) {
        return x % y;
    } else {
        return 0;
    }
}

long eval_rpn(mpc_ast_t *node) {
    // Handle base case when leaf contains only a number
    if (strstr(node->tag, "num")) {
        return atoi(node->contents);
    }

    // Opeartor is always the first child
    char *op = node->children[1]->contents;

    // Evaluate the first expression
    // Setting res to 0, 1 will give wrong result for mul/add respectively.
    long res = eval_rpn(node->children[2]);

    // Recurse for later expressions
    int i = 3;
    while (strstr(node->children[i]->tag, "expr")) {
        res = calc(op, res, eval_rpn(node->children[i]));
        i++;
    }

    return res;
}
