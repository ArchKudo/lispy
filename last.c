#include "last.h"
#include <stdlib.h>
#include <string.h>
#include "lstruct.h"
#include "mpc.h"
#include "lwrap.h"
#include "lval.h"

LVal *lval_read_long(mpc_ast_t *node) {
    errno = 0;
    long val = strtol(node->contents, NULL, 10);
    return errno != ERANGE ? lval_wrap_long(val)
                           : lval_wrap_err("Number too large!");
}

LVal *lval_read_str(mpc_ast_t *node) {
    // Chops of ending quote
    node->contents[strlen(node->contents) - 1] = '\0';

    // Copies string without starting quote
    char *unescaped = malloc(strlen(node->contents + 1) + 1);
    unescaped = strcpy(unescaped, node->contents + 1);

    // Unescape string
    unescaped = mpcf_unescape(unescaped);

    LVal *lstr = lval_wrap_str(unescaped);

    free(unescaped);
    return lstr;
}

LVal *lval_read_ast(mpc_ast_t *node) {
    if (strstr(node->tag, "num")) {
        return lval_read_long(node);
    }
    if (strstr(node->tag, "sym")) {
        return lval_wrap_sym(node->contents);
    }

    if (strstr(node->tag, "str")) {
        return lval_read_str(node);
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
            (strcmp(node->children[i]->tag, "regex") == 0) ||
            (strstr(node->children[i]->tag, "comment"))) {
            continue;
        } else {
            root = lval_add(root, lval_read_ast(node->children[i]));
        }
    }
    return root;
}
