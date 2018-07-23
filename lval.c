#include "lval.h"
#include <errno.h>
#include <string.h>
#include "mpc.h"

#define MAX_ERR 512

#define LASSERT(args, cond, err)   \
    if (!(cond)) {                 \
        lval_del(args);            \
        return lval_wrap_err(err); \
    }

///////////////////////////////////////////////////////////////////////////////
/* Function Declarations */
///////////////////////////////////////////////////////////////////////////////

/* LVal Wrapper functions */

/**
 * @brief  Convert a long to a LVal
 * @param  val: A long to be converted to a LVal
 * @retval A LVal with num field set and type LVAL_NUM
 */
LVal *lval_wrap_long(long val);

/**
 * @brief  Wrap strings as LVal's
 * @param  type: Type of LVal (from enum)
 * @param  *str: String to wrap
 * @retval A LVal of type: `type`
 */
LVal *lval_wrap_str(int type, char *str);

// lval_wrap_str with type LVAL_SYM
LVal *lval_wrap_sym(char *sym);

/**
 * @brief  Wrap error with variable arguments to LVal
 * @note Copies a maximum of MAX_ERR bytes including null,
 *       may lead to truncated error string
 * @param *fmt: Format of the given variable argument error string
 * @retval A LVal of type LVAL_ERR
 */
LVal *lval_wrap_err(char *fmt, ...);

/**
 * @brief  Wrap S/Q-Expressions
 * @param  type: Type of expression
 * @retval A LVal of type: `type`
 */
LVal *lval_wrap_expr(int type);

// lval_wrap_expr with type LVAL_SEXPR
LVal *lval_wrap_sexpr(void);

// lval_wrap_expr with type LVAL_QEXPR
LVal *lval_wrap_qexpr(void);

/**
 * @brief  Wrap a LBuiltin as an LVal
 * @param  fun: A LBuiltin
 * @retval A LVal with type LVAL_FUN
 */
LVal *lval_wrap_fun(LBuiltin fun);
/* LVal methods for modification and evaluation */

/**
 * @brief  Delete a LVal
 * @param  *lval: The LVal which need to be freed along with its contents
 * @retval None
 */
void lval_del(LVal *lval);

/**
 * @brief  Add a LVal to another LVal
 * @param  *parent: The parent LVal, the child is added to this LVal
 * @param  *child: The child LVal which needs to be added
 * @retval Returns the parent LVal
 */
LVal *lval_add(LVal *parent, LVal *child);

/**
 * @brief  Add an LVal to the end of other
 * @param  *augend: The parent to which the other LVal is added
 * @param  *addend: The child which is joined with augend
 * @retval The joined LVal (augend += addend)
 */
LVal *lval_join(LVal *augend, LVal *addend);

/**
 * @brief  Pop child at ith position
 * @param  *lval: Parent LVal
 * @param  index: Location of child
 * @retval The popped child
 */
LVal *lval_pop(LVal *lval, int index);

/**
 * @brief  Get child at ith position, delete rest
 * @param  *lval: The parent lval
 * @param  index: Position of child
 * @retval Child at ith position
 */
LVal *lval_take(LVal *lval, int index);

/**
 * @brief  Evaluates RPN ast recursively
 * @param  *node: Parent/Root node of tree
 * @retval Result of expression
 */
LVal *lval_eval(LVal *lval);

/**
 * @brief  Evaluate a S-Expression
 * @param  *lval: An LVal of type LVAL_SEXPR
 * @retval Result wrapped as a LVal
 */
LVal *lval_eval_sexpr(LVal *lval);

/* MPC AST handlers */

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

/* LVal printing methods */

/**
 * @brief Print value of LVal adding a new line at the end
 * @param  val: An LVal
 * @retval None
 */
void lval_println(LVal *lval);

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
void lval_print_expr(LVal *lval, char open, char close);

// lval_print_expr wrapper for S-Expressions
void lval_print_sexpr(LVal *lval);
// lval_print_expr wrapper for Q-Expressions
void lval_print_qexpr(LVal *lval);

/**
 * @brief  Print type of LVal
 * @param  type: Takes integer value of type for LVAL_TYPE enum
 * @retval String representing the LVAL_TYPE
 */
char *lval_print_type(int type);

/* LVal Builtins */

/**
 * @brief  Evaluate operation LVAL_SYM between LVAL_NUM
 * @param  *lval: LVal containing numbers as children
 * @param  *op: Corresponding operator as string for the operation
 * @retval Result wrapped as LVal
 */
LVal *builtin_op(LVal *lval, char *op);

/**
 * @brief  Perform operation based on operator string
 * @param  *lval: LVal on which operation is to be performed
 * @param  *op_str: Operator as a string
 * @retval Resultant LVal
 */
LVal *builtin_table(LVal *lval, char *op_str);

/**
 * @brief  Returns head of a qexpr
 * @note   eg: head {0 1 2} => 0
 * @param  *lval: A lval with type qexpr and atleast one child
 * @retval Head of qexpr
 */
LVal *builtin_head(LVal *lval);

/**
 * @brief  Return the rest of the qexpr without the head
 * @note   eg: tail {0 1 2} => {1 2}
 * @param  *lval: A lval with type qexpr and atleast one child
 * @retval Tail of qexpr
 */
LVal *builtin_tail(LVal *lval);

/**
 * @brief Convert a LVal sequence to qexpr
 * @param  *lval: A LVal
 * @retval Returns an LVal of type LVAL_QEXPR
 */
LVal *builtin_list(LVal *lval);

/**
 * @brief  Evaluate a qexpr
 * @note   Evaluates a qexpr as a sexpr using lval_eval
 * @param  *lval: A qexpr
 * @retval Value of qexpr
 */
LVal *builtin_eval(LVal *lval);

/**
 * @brief  Join multiple qexpr
 * @param  *lval: A lval of type LVAL_QEXPR and children of type LVAL_QEXPR
 * @retval The joined lval
 */
LVal *builtin_join(LVal *lval);

///////////////////////////////////////////////////////////////////////////////
/* Functions to wrap primitives as LVal */
///////////////////////////////////////////////////////////////////////////////

LVal *lval_wrap_long(long num) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = LVAL_NUM;
    lval->num = num;
    return lval;
}

LVal *lval_wrap_str(int type, char *str) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = type;
    if (type == LVAL_ERR) {
        // strlen return length excluding null byte terminator '\0'
        // [len] + 1 ensures space for the null byte
        lval->err = malloc(strlen(str) + 1);
        // Copy str to lval->err including null byte
        strcpy(lval->err, str);
    } else if (type == LVAL_SYM) {
        lval->sym = malloc(strlen(str) + 1);
        strcpy(lval->sym, str);
    }
    return lval;
}

LVal *lval_wrap_sym(char *sym) { return lval_wrap_str(LVAL_SYM, sym); }

LVal *lval_wrap_err(char *fmt, ...) {
    LVal *lerr = malloc(sizeof(LVal));
    lerr->type = LVAL_ERR;

    // Creating a variable list to store the arguments
    va_list va;

    // Initialize the va_list
    // va_start takes two arguments:
    // The va_list and the last argument before ellipsis
    va_start(va, fmt);

    // Allocate maximum size of error string
    lerr->err = malloc(sizeof(MAX_ERR));

    // Copy the va string to lerr->err atmost MAX_ERR bytes including null
    // Leaving space for null byte is therefore required
    vsnprintf(lerr->err, MAX_ERR - 1, fmt, va);

    // Resize(Reduce) lerr->err to size of va string from MAX_ERR
    // Again here, strlen doesn't output size of string including null byte
    lerr->err = realloc(lerr->err, strlen(lerr->err) + 1);

    // Free va_list
    va_end(va);

    return lerr;
}

LVal *lval_wrap_expr(int type) {
    LVal *lval = malloc(sizeof(LVal));
    lval->type = type;
    lval->child_count = 0;
    lval->children = NULL;
    return lval;
}

LVal *lval_wrap_sexpr(void) { return lval_wrap_expr(LVAL_SEXPR); }
LVal *lval_wrap_qexpr(void) { return lval_wrap_expr(LVAL_QEXPR); }

LVal *lval_wrap_fun(LBuiltin fun) {
    LVal *lfun = malloc(sizeof(LVal));
    lfun->type = LVAL_FUN;
    lfun->fun = fun;
    return lfun;
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to operate on LVal struct */
///////////////////////////////////////////////////////////////////////////////

void lval_del(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
        case LVAL_FUN:
            break;
        case LVAL_ERR:
            free(lval->err);
            break;
        case LVAL_SYM:
            free(lval->sym);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < lval->child_count; i++) {
                lval_del(lval->children[i]);
            }
            free(lval->children);
            break;
    }
    free(lval);
}

LVal *lval_copy(LVal *lval) {
    LVal *copy = malloc(sizeof(LVal));
    copy->type = lval->type;
    switch (copy->type) {
        case LVAL_NUM:
            copy->num = lval->num;
            break;
        case LVAL_FUN:
            copy->fun = lval->fun;
            break;
        case LVAL_SYM:
            copy->sym = malloc(strlen(lval->sym) + 1);
            strcpy(copy->sym, lval->sym);
            break;
        case LVAL_ERR:
            copy->err = malloc(strlen(lval->err) + 1);
            strcpy(copy->err, lval->err);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            copy->child_count = lval->child_count;
            for (int i = 0; i < copy->child_count; i++) {
                copy->children[i] = lval->children[i];
            }
            break;
    }
    return copy;
}

LVal *lval_add(LVal *parent, LVal *child) {
    parent->child_count += 1;
    parent->children =
        realloc(parent->children, sizeof(LVal *) * parent->child_count);

    // Last child is child_count - 1 due to 0-based indexing
    parent->children[parent->child_count - 1] = child;
    return parent;
}

LVal *lval_join(LVal *augend, LVal *addend) {
    for (int i = 0; i < addend->child_count; i++) {
        augend = lval_add(augend, addend->children[i]);
    }
    free(addend->children);
    free(addend);
    return augend;
}

LVal *lval_pop(LVal *lval, int index) {
    LVal *popped = lval->children[index];

    // Overwrite the child at `index (to end)` with `index + 1 (to end)`
    memmove(&lval->children[index],      // Destination
            &lval->children[index + 1],  // Source
            (size_t)(sizeof(LVal *) * (lval->child_count - index - 1))  // Size
    );

    lval->child_count -= 1;

    // Resize the `lval->children` array to the new size (i.e child_count - 1)
    lval->children =
        realloc(lval->children, sizeof(LVal *) * lval->child_count);

    // Return LVal at the ith position
    return popped;
}

LVal *lval_take(LVal *lval, int index) {
    // Pop LVal at the given index
    LVal *popped = lval_pop(lval, index);

    // Delete the rest LVal
    lval_del(lval);

    // Return the popped value
    return popped;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LVal *lval_read_long(mpc_ast_t *node) {
    errno = 0;
    long val = strtol(node->contents, NULL, 10);
    return errno != ERANGE ? lval_wrap_long(val)
                           : lval_wrap_err("Number too large!");
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
    } else if (strstr(node->tag, "qexpr")) {
        root = lval_wrap_qexpr();
    }

    for (int i = 0; i < node->children_num; i++) {
        if ((strcmp(node->children[i]->contents, "(") == 0) ||
            (strcmp(node->children[i]->contents, ")") == 0) ||
            (strcmp(node->children[i]->contents, "{") == 0) ||
            (strcmp(node->children[i]->contents, "}") == 0) ||
            (strcmp(node->children[i]->tag, "regex") == 0)) {
            continue;
        } else {
            root = lval_add(root, lval_read_ast(node->children[i]));
        }
    }
    return root;
}

///////////////////////////////////////////////////////////////////////////////
/* Functions to print LVal's */
///////////////////////////////////////////////////////////////////////////////

void lval_print(LVal *lval) {
    switch (lval->type) {
        case LVAL_NUM:
            printf("%ld", lval->num);
            break;
        case LVAL_FUN:
            printf("<function>");
            break;
        case LVAL_ERR:
            printf("Error: %s", lval->err);
            break;
        case LVAL_SYM:
            printf("%s", lval->sym);
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

void lval_println(LVal *lval) {
    lval_print(lval);
    printf("\n");
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
        case LVAL_QEXPR:
            return "Quoted Expression";
        case LVAL_SEXPR:
            return "Symbolic Expression";
        default:
            return "Unknown type";
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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
    LVal *result = builtin_table(lval, first->sym);

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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

LVal *builtin_table(LVal *lval, char *op_str) {
    if (strcmp("list", op_str) == 0) {
        return builtin_list(lval);
    } else if (strcmp("head", op_str) == 0) {
        return builtin_head(lval);
    } else if (strcmp("tail", op_str) == 0) {
        return builtin_tail(lval);
    } else if (strcmp("eval", op_str) == 0) {
        return builtin_eval(lval);
    } else if (strcmp("join", op_str) == 0) {
        return builtin_join(lval);
    } else if (strstr("+-/*%", op_str)) {
        return builtin_op(lval, op_str);
    }
    lval_del(lval);
    return lval_wrap_err("Can't determine type of operation!");
}

LVal *builtin_head(LVal *lval) {
    // First sym is not a child
    LASSERT(lval, lval->child_count == 1,
            "\"head\" expected a single argument!");

    LASSERT(lval, lval->children[0]->type == LVAL_QEXPR,
            "\"head\" expected a Q-Expression as argument!");

    LASSERT(lval, lval->children[0]->child_count != 0,
            "\"head\" requires a non-empty Q-Expression!");

    LVal *qexpr = lval_take(lval, 0);

    while (qexpr->child_count > 1) {
        lval_del(lval_pop(qexpr, 1));
    }

    return qexpr;
}

LVal *builtin_tail(LVal *lval) {
    LASSERT(lval, lval->child_count == 1,
            "\"tail\" expected a single argument!");

    LASSERT(lval, lval->children[0]->type == LVAL_QEXPR,
            "\"tail\" expected a Q-Expression as argument!");

    LASSERT(lval, lval->children[0]->child_count != 0,
            "\"tail\" requires a non-empty Q-Expression!");

    LVal *qexpr = lval_take(lval, 0);

    lval_del(lval_pop(qexpr, 0));
    return qexpr;
}

LVal *builtin_list(LVal *lval) {
    lval->type = LVAL_QEXPR;
    return lval;
}

LVal *builtin_eval(LVal *lval) {
    LASSERT(lval, lval->child_count == 1,
            "\"eval\" expected a single argument");

    LASSERT(lval, lval->child_count == 1,
            "\"eval\" expected a Q-Expression as argument");

    LVal *qexpr = lval_take(lval, 0);
    qexpr->type = LVAL_SEXPR;
    return lval_eval(qexpr);
}

LVal *builtin_join(LVal *lval) {
    for (int i = 0; i < lval->child_count; i++) {
        LASSERT(lval, lval->children[i]->type == LVAL_QEXPR,
                "\"join\" expected a Q-Expression");
    }

    LVal *qexpr = lval_pop(lval, 0);

    while (lval->child_count) {
        qexpr = lval_join(qexpr, lval_pop(lval, 0));
    }

    lval_del(lval);
    return qexpr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
