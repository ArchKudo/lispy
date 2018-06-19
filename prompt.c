// clang-format off
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
// clang-format on
#include <stdlib.h>
#include <string.h>

#include "mpc.h"

#define TRUE 1
#define FALSE 0

int calculate_nodes(mpc_ast_t *root) {
    if (root->children_num == 0) {
        return 1;
    }

    int total = 1;
    for (int i = 0; i < root->children_num; i++) {
        total += calculate_nodes(root->children[i]);
    }

    return total;
}

long eval_op(long x, char *op, long y) {
    if (strcmp(op, "+") == 0) {
        return x + y;
    }
    if (strcmp(op, "-") == 0) {
        return x - y;
    }
    if (strcmp(op, "*") == 0) {
        return x * y;
    }
    if (strcmp(op, "/") == 0) {
        return x / y;
    }
    if (strcmp(op, "%") == 0) {
        return x % y;
    }
    return 0;
}

long eval_rpn_ast(mpc_ast_t *node) {
    if (strstr(node->tag, "num")) {
        return atoi(node->contents);
    }

    char *op = node->children[1]->contents;

    long res = eval_rpn_ast(node->children[2]);

    int i = 3;
    while (strstr(node->children[i]->tag, "expr")) {
        res = eval_op(res, op, eval_rpn_ast(node->children[i]));
        i++;
    }

    return res;
}

int main() {
    // Create parsers
    mpc_parser_t *Number = mpc_new("num");
    mpc_parser_t *Operator = mpc_new("op");
    mpc_parser_t *Expression = mpc_new("expr");
    mpc_parser_t *Notation = mpc_new("rpn");

    // Define the above parsers with patterns + regex
    const char *lang =
        " \
                num   : /-?[0-9]+/ ; \
                op : '+' | '-' | '*' | '/' | '%' ; \
                expr     : <num> | '(' <op> <expr>+ ')' ; \
                rpn    : /^/ <op> <expr>+ /$/ ; \
            ";
    mpca_lang(MPCA_LANG_DEFAULT, lang, Number, Operator, Expression, Notation);

    char *input = NULL;

    printf("LISPY v0.0.3\n");
    printf("Enter CTRL+C or, CTRL+D on an empty line to exit\n");
    while (TRUE) {
        mpc_result_t result;
        input = readline("lispy> ");
        if (input != NULL) {
            add_history(input);
        } else {  // Handle EOF
            printf("Bye!");
            break;
        }

        if (mpc_parse("<stdin>", input, Notation, &result)) {
            mpc_ast_print(result.output);
            mpc_ast_t *ast = result.output;
            printf("For Parent,\n");
            printf("\tTag: %s\n", ast->tag);
            printf("\tContents: %s\n", ast->contents);
            printf("\tNumber of Children: %i\n", ast->children_num);
            printf("\tTotal number of nodes: %d\n", calculate_nodes(ast));
            mpc_ast_t *child = NULL;
            for (int i = 0; i < ast->children_num; i++) {
                child = ast->children[i];
                printf("For Child %d,\n", i + 1);
                printf("\tTag: %s\n", child->tag);
                printf("\tContents: %s\n", child->contents);
                printf("\tNumber of Children: %d\n", child->children_num);
            }
            printf("==================================\n");
            printf("Result: %ld\n", eval_rpn_ast(result.output));
            mpc_ast_delete(result.output);
        } else {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }
    }
    mpc_cleanup(4, Number, Operator, Expression, Notation);
    free(input);
    return 0;
}
