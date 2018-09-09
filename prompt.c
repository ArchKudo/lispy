#include <editline/readline.h>

#include "lispy.h"
#include "mpc.h"
#include "parser.h"

#define TRUE 1
#define FALSE 0

mpc_parser_t *Number;
mpc_parser_t *Symbol;
mpc_parser_t *String;
mpc_parser_t *Comment;
mpc_parser_t *Expression;
mpc_parser_t *QExpression;
mpc_parser_t *SExpression;
mpc_parser_t *Lisp;

int main(int argc, char *argv[]) {
    // Create parsers
    Number = mpc_new("num");
    Symbol = mpc_new("sym");
    String = mpc_new("str");
    Comment = mpc_new("comment");
    SExpression = mpc_new("sexpr");
    QExpression = mpc_new("qexpr");
    Expression = mpc_new("expr");
    Lisp = mpc_new("lisp");

    // Define the above parsers with patterns + regex
    const char *lang =
        " \
        num: /-?[0-9]+/ ; \
        sym: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%]+/ ; \
        str: /\"(\\\\.|[^\"])*\"/ ; \
        comment: /;[^\\n^\\r]*/ ; \
        sexpr: '(' <expr>* ')' ; \
        qexpr: '{' <expr>* '}' ; \
        expr: <num> | <sym> | <str> | <comment> | <sexpr> | <qexpr> ; \
        lisp: /^/ <expr>* /$/ ; \
    ";
    mpca_lang(MPCA_LANG_DEFAULT, lang, Number, Symbol, String, Comment,
              SExpression, QExpression, Expression, Lisp);

    static char *input = (char *)NULL;

    printf("LISPY v0.0.10\n");
    printf("Enter CTRL+C or, CTRL+D on an empty line to exit\n");

    // Create the parent LEnv
    LEnv *lparent = lenv_new();

    // Add the default builtins to the parent LEnv
    lenv_init_builtins(lparent);
    if (argc == 1) {
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
                printf("\nBye!");
                break;
            }

            if (mpc_parse("<stdin>", input, Lisp, &result)) {
                LVal *lval = lval_eval(lparent, lval_read_ast(result.output));
                lval_println(lval);
                lval_del(lval);
                mpc_ast_delete(result.output);
            } else {
                mpc_err_print(result.error);
                mpc_err_delete(result.error);
            }
        }
    } else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            LVal *largs = lval_add(lval_wrap_sexpr(), lval_wrap_str(argv[i]));
            LVal *lfile = builtin_load(lparent, largs);
            if (lfile->type == LVAL_ERR) {
                lval_println(lfile);
            }
            lval_del(lfile);
        }
    }

    lenv_del(lparent);

    mpc_cleanup(8, Number, Symbol, String, Comment, SExpression, QExpression,
                Expression, Lisp);
    free(input);
    return 0;
}
