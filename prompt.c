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
        } else  // Handle EOF
        {
            printf("Bye!");
            break;
        }

        if (mpc_parse("<stdin>", input, Notation, &result)) {
            mpc_ast_print(result.output);
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
