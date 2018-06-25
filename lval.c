#include "lval.h"
#include <errno.h>
#include <string.h>
#include "mpc.h"

/**
 * @brief  Convert a long to a LVal
 * @param  val: A long to be converted to a LVal
 * @retval A LVal with num field set and type LVAL_NUM
 */
LVal *lval_wrap_long(long val);

/**
 * @brief  Convert an error string to a LVal
 * @param  *err: Description of error as a string
 * @retval LVal with err field set and type LVAL_ERR
 */
LVal *lval_wrap_err(char *err);

/**
 * @brief  Convert an symbol string to a LVal
 * @param  *sym: The symbol string
 * @retval LVal with sym field set and type LVAL_SYM
 */
LVal *lval_wrap_sym(char *sym);

/**
 * @brief  Create a new SExpression
 * @retval LVal with sexpr field set and type LVAL_SEXPR
 */
LVal *lval_wrap_sexpr(void);

/**
 * @brief  Add a LVal to another LVal
 * @param  *parent: The parent LVal, the child is added to this LVal
 * @param  *child: The child LVal which needs to be added
 * @retval Returns the parent LVal
 */
LVal *lval_add(LVal *parent, LVal *child);

/**
 * @brief  Read ast as a LVal
 * @param  *node: The ast to be converted
 * @retval A LVal
 */
LVal *lval_read_ast(mpc_ast_t *node);

/**
 * @brief  Handler for lval_wrap_long
 * @note   Converts string to long converts to LVal on success
 * @param  *node: ast node containing a long number as contents
 * @retval A LVal of type LVAL_NUM on success, LVAL_ERR otherwise
 */
LVal *lval_read_long(mpc_ast_t *node);

/**
 * @brief Print value of LVal
 * @param  val: An LVal
 * @retval None
 */
void lval_print(LVal *lval);

/**
 * @brief Print an expression
 * @param  *lval: Expression as an LVal
 * @param  open: Character to be inserted before expression
 * @param  close: Character to be inserted at end of expression
 * @retval None
 */
void lval_print_expr(LVal *lval, char open, char close);

LVal *lval_wrap_long(long val) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_NUM;
    lval->num = val;
    return lval;
}

LVal *lval_wrap_err(char *err) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_ERR;
    lval->err = malloc(strlen(err) + 1);
    strcpy(lval->err, err);
    return lval;
}

LVal *lval_wrap_sym(char *sym) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_SYM;
    lval->sym = malloc(strlen(sym) + 1);
    strcpy(lval->sym, sym);
    return lval;
}

LVal *lval_wrap_sexpr(void) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_SEXPR;
    lval->children_count = 0;
    lval->children = NULL;
    return lval;
}

void lval_del(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
            break;
        case LVAL_ERR:
            free(lval->err);
            break;
        case LVAL_SYM:
            free(lval->sym);
            break;
        case LVAL_SEXPR:
            for (int i = 0; i < lval->children_count; i++) {
                lval_del(lval->children[i]);
            }
            free(lval->children);
            break;
    }
    free(lval);
}

LVal *lval_read_long(mpc_ast_t *node) {
    errno = 0;
    long val = strtol(node->contents, NULL, 10);
    return errno != ERANGE ? lval_wrap_long(val)
                           : lval_wrap_err("Number too large!");
}

LVal *lval_add(LVal *parent, LVal *child) {
    parent->children_count += 1;
    parent->children =
        realloc(parent->children, sizeof(LVal *) * parent->children_count);
    parent->children[parent->children_count - 1] = child;
    return parent;
}

LVal *lval_read_ast(mpc_ast_t *node) {
    if (strstr(node->tag, "num")) {
        return lval_read_long(node);
    }
    if (strstr(node->tag, "sym")) {
        return lval_wrap_sym(node->contents);
    }

    LVal *root = NULL;
    if ((strcmp(node->tag, ">") == 0) || (strstr(node->tag, "sexpr"))) {
        root = lval_wrap_sexpr();
    }

    for (int i = 0; i < node->children_num; i++) {
        if ((strcmp(node->children[i]->contents, "(") == 0) ||
            (strcmp(node->children[i]->contents, ")") == 0) ||
            (strcmp(node->children[i]->tag, "regex") == 0)) {
            continue;
        } else {
            root = lval_add(root, lval_read_ast(node->children[i]));
        }
    }
    return root;
}

void lval_print(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
            printf("%ld", lval->num);
            break;
        case LVAL_ERR:
            printf("Error: %s!", lval->err);
            break;
        case LVAL_SYM:
            printf("%s", lval->sym);
            break;
        case LVAL_SEXPR:
            lval_print_expr(lval, '(', ')');
            break;

        default:
            break;
    }
}

void lval_print_expr(LVal *lval, char open, char close) {
    printf("%c", open);
    for (int i = 0; i < lval->children_count; i++) {
        lval_print(lval->children[i]);

        if (i != (lval->children_count - 1)) {
            printf(" ");
        }
    }
    printf("%c", close);
}

void lval_println(LVal *lval) {
    lval_print(lval);
    printf("\n");
}

// LVal calc(char *op, LVal x, LVal y) {
//     if (x.type == LVAL_ERR) {
//         return x;
//     }

//     if (y.type == LVAL_ERR) {
//         return y;
//     }

//     if (strcmp(op, "+") == 0) {
//         return lval_wrap_num(x.num + y.num);
//     } else if (strcmp(op, "-") == 0) {
//         return lval_wrap_num(x.num - y.num);
//     } else if (strcmp(op, "*") == 0) {
//         return lval_wrap_num(x.num * y.num);
//     } else if (strcmp(op, "/") == 0) {
//         return y.num == 0 ? lval_wrap_err(LERR_DIV_ZERO)
//                           : lval_wrap_num(x.num / y.num);
//     } else if (strcmp(op, "%") == 0) {
//         return lval_wrap_num(x.num % y.num);
//     } else {
//         return lval_wrap_err(LERR_BAD_OP);
//     }
// }

// LVal eval(mpc_ast_t *node) {
//     // Handle base case when leaf contains only a number
//     if (strstr(node->tag, "num")) {
//         errno = 0;
//         // Convert contents to long with base 10
//         long num = strtol(node->contents, NULL, 10);
//         return errno != ERANGE ? lval_wrap_num(num)
//                                : lval_wrap_err(LERR_BAD_NUM);
//     }

//     // Opeartor is always the first child
//     char *op = node->children[1]->contents;

//     // Evaluate the first expression
//     // Setting res to 0, 1 will give wrong result for mul/add respectively.
//     LVal res = eval(node->children[2]);

//     // Recurse for later expressions
//     int i = 3;
//     while (strstr(node->children[i]->tag, "expr")) {
//         res = calc(op, res, eval(node->children[i]));
//         i++;
//     }

//     return res;
// }

// void print_lval(LVal lval) {
//     switch (lval.type) {
//         case LVAL_NUM:
//             printf("%ld\n", lval.num);
//             break;

//         case LVAL_ERR:
//             switch (lval.err) {
//                 case LERR_DIV_ZERO:
//                     printf("Error cannot divide number with zero!\n");
//                     break;
//                 case LERR_BAD_OP:
//                     printf("Error invalid operator!\n");
//                     break;
//                 case LERR_BAD_NUM:
//                     printf("Error can't decide type of number!\n");
//                     break;

//                 default:
//                     printf("Unspecfied error!");
//             }
//             break;

//         default:
//             break;
//     }
// }
