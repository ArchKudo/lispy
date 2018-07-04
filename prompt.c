// clang-format off
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
// clang-format on
#include "lval.h"
#include "mpc.h"

#define TRUE 1
#define FALSE 0

int main() {
    // Create parsers
    mpc_parser_t *Number = mpc_new("num");
    mpc_parser_t *Symbol = mpc_new("sym");
    mpc_parser_t *Expression = mpc_new("expr");
    mpc_parser_t *QExpression = mpc_new("qexpr");
    mpc_parser_t *SExpression = mpc_new("sexpr");
    mpc_parser_t *Notation = mpc_new("lisp");

    // Define the above parsers with patterns + regex
    const char *lang =
        " \
                num: /-?[0-9]+/ ; \
                sym: '+' | '-' | '*' | '/' | '%' | \
                    \"list\" | \"head\" | \"tail\" | \"join\" | \"eval\" ; \
                sexpr: '(' <expr>* ')' ; \
                qexpr: '{' <expr>* '}' ; \
                expr: <num> | <sym> | <sexpr> | <qexpr> ; \
                lisp: /^/ <expr>* /$/ ; \
            ";
    mpca_lang(MPCA_LANG_DEFAULT, lang, Number, Symbol, SExpression, QExpression,
              Expression, Notation);

    static char *input = (char *)NULL;

    printf("LISPY v0.0.5\n");
    printf("Enter CTRL+C or, CTRL+D on an empty line to exit\n");
    while (TRUE) {
        mpc_result_t result;

        if (input) {
            free(input);
            input = (char *)NULL;
        }

        input = readline("lispy> ");

        if (input) {
            add_history(input);
        } else {  // Handle EOF
            printf("Bye!");
            break;
        }

        if (mpc_parse("<stdin>", input, Notation, &result)) {
            LVal *lval = lval_eval(lval_read_ast(result.output));
            lval_println(lval);
            lval_del(lval);
            mpc_ast_delete(result.output);
        } else {
            mpc_err_print(result.error);
            mpc_err_delete(result.error);
        }
    }
    mpc_cleanup(6, Number, Symbol, SExpression, Expression, QExpression,
                Notation);
    free(input);
    return 0;
}
