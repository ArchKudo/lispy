#ifndef PARSER_H
#define PARSER_H

#include "mpc.h"

extern mpc_parser_t *Number;
extern mpc_parser_t *Symbol;
extern mpc_parser_t *String;
extern mpc_parser_t *Comment;
extern mpc_parser_t *Expression;
extern mpc_parser_t *QExpression;
extern mpc_parser_t *SExpression;
extern mpc_parser_t *Notation;

#endif
