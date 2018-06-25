#include "lval.h"
#include <errno.h>
#include <string.h>
#include "mpc.h"

/*
 * Public functions:
 * 1. lval_println
 * 2. lval_read_ast
 * 3. lval_del
 * 4. lval_eval
 *
 * Private function:
 * 1. lval_wrap_long
 * 2. lval_wrap_err
 * 3. lval_wrap_sym
 * 4. lval_wrap_sexpr
 * 5. lval_add
 * 6. lval_read_long
 * 7. lval_print
 * 8. lval_print_sexpr
 * 9. lval_take
 * 10. lval_pop
 * 11. builtin_op
 * 12. lval_eval_sexpr
 */

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
 * @brief  Handler for lval_wrap_long
 * @note   Converts string to long converts to LVal on success
 * @param  *node: ast node containing a long number as contents
 * @retval A LVal of type LVAL_NUM on success, LVAL_ERR otherwise
 */
LVal *lval_read_long(mpc_ast_t *node);

/**
 * @brief Print value of LVal
 * @note Switches between different types of LVal and prints appropriately
 * @param  val: An LVal
 * @retval None
 */
void lval_print(LVal *lval);

/**
 * @brief Print an S-Expression
 * @note lval_print handler for S-Expression
 * @param  *lval: LVal with type LVAL_SEXPR
 * @param  open: Character to be inserted before S-Expression
 * @param  close: Character to be inserted at end of S-Expression
 * @retval None
 */
void lval_print_sexpr(LVal *lval, char open, char close);

/**
 * @brief  Get child at ith position, delete rest
 * @param  *lval: The parent lval
 * @param  i: Position of child
 * @retval Child at ith position
 */
LVal *lval_take(LVal *lval, int i);

/**
 * @brief  Pop child at ith position
 * @param  *lval: Parent LVal
 * @param  i: Location of child
 * @retval The popped child
 */
LVal *lval_pop(LVal *lval, int i);

/**
 * @brief  Evaluate operation LVAL_SYM between LVAL_NUM
 * @param  *lval: LVal containing numbers as children
 * @param  *op: Corresponding operator as string for the operation
 * @retval Result wrapped as LVal
 */
LVal *builtin_op(LVal *lval, char *op);

/**
 * @brief  Evaluate a S-Expression
 * @param  *lval: An LVal of type LVAL_SEXPR
 * @retval Result wrapped as a LVal
 */
LVal *lval_eval_sexpr(LVal *lval);

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
    lval->child_count = 0;
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
            for (int i = 0; i < lval->child_count; i++) {
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
    parent->child_count += 1;
    parent->children =
        realloc(parent->children, sizeof(LVal *) * parent->child_count);
    parent->children[parent->child_count - 1] = child;
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
            lval_print_sexpr(lval, '(', ')');
            break;

        default:
            break;
    }
}

void lval_print_sexpr(LVal *lval, char open, char close) {
    printf("%c", open);
    for (int i = 0; i < lval->child_count; i++) {
        lval_print(lval->children[i]);

        if (i != (lval->child_count - 1)) {
            printf(" ");
        }
    }
    printf("%c", close);
}

void lval_println(LVal *lval) {
    lval_print(lval);
    printf("\n");
}

LVal *lval_eval_sexpr(LVal *lval) {
    for (int i = 0; i < lval->child_count; i++) {
        lval->children[i] = lval_eval(lval->children[i]);
    }

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
    LVal *first = lval_pop(lval, 0);

    // Raise error if first child isn't a symbol
    // Also free's first child and lval
    if (first->type != LVAL_SYM) {
        lval_del(first);
        lval_del(lval);
        return lval_wrap_err("S-Expression doesn't starts with a symbol!");
    }

    // Operate on LVal, after extracting first child(symbol)
    LVal *result = builtin_op(lval, first->sym);

    // Delete the first child
    lval_del(first);
    return result;
}

LVal *lval_eval(LVal *lval) {
    if (lval->type == LVAL_SEXPR) {
        return lval_eval_sexpr(lval);
    }
    return lval;
}

LVal *lval_pop(LVal *lval, int i) {
    LVal *popped = lval->children[i];

    // Push memory location of n+1th to that of nth
    // i.e |i|i+1|..|n| -> |i+1|..|n|
    //      ^- Old          ^- New starting address of allocated memory
    memmove(&lval->children[i], &lval->children[i + 1],
            (size_t)(sizeof(LVal *) * (lval->child_count - i - 1)));

    lval->child_count -= 1;

    // Resize the `lval->children` array to the new size (i.e child_count - 1)
    lval->children =
        realloc(lval->children, sizeof(LVal *) * lval->child_count);

    // Return LVal at the ith position
    return popped;
}

LVal *lval_take(LVal *lval, int i) {
    // Pop LVal at ith index
    LVal *popped = lval_pop(lval, i);
    // Delete the rest
    lval_del(lval);
    // Return the popped value
    return popped;
}

LVal *builtin_op(LVal *lval, char *op) {
    // Error handling for non-number values
    for (int i = 0; i < lval->child_count; i++) {
        if (lval->children[i]->type != LVAL_NUM) {
            lval_del(lval);
            return lval_wrap_err("Cannot operate on non-number!");
        }
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

        lval_del(second);
    }

    lval_del(lval);
    return first;
}
