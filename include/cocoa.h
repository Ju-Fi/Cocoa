#ifndef COCOA_H_
#define COCOA_H_

#include "stb_ds.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Tokens */

#define TOKEN_TYPES                             \
    X(TT_NIL)                                   \
    X(TT_INT)                                   \
    X(TT_FLOAT)                                 \
    X(TT_BOOL)                                  \
    X(TT_STR)                                   \
                                                \
    X(TT_IDEN)                                  \
    X(TT_KWRD_MACRO)                            \
    X(TT_KWRD_IF)                               \
    X(TT_KWRD_ELSE)                             \
    X(TT_KWRD_WHILE)                            \
    X(TT_KWRD_DO)                               \
    X(TT_KWRD_AND)                              \
    X(TT_KWRD_OR)                               \
    X(TT_KWRD_DUP)                              \
    X(TT_KWRD_DROP)                             \
    X(TT_KWRD_SWAP)                             \
    X(TT_KWRD_OVER)                             \
    X(TT_KWRD_ROT)                              \
                                                \
    X(TT_TERM)                                  \
                                                \
    X(TT_OP_ADD)                                \
    X(TT_OP_SUB)                                \
    X(TT_OP_MUL)                                \
    X(TT_OP_DIV)                                \
    X(TT_OP_MOD)                                \
    X(TT_OP_GT)                                 \
    X(TT_OP_LT)                                 \
    X(TT_OP_GTEQ)                               \
    X(TT_OP_LTEQ)                               \
    X(TT_OP_EQ)                                 \
    X(TT_OP_NOT)                                \
    X(TT_OP_NEQ)                                \
                                                \
    X(TT_OP_PRINTLN)
#define X(x) x,

enum TokenType {
TOKEN_TYPES
};
#undef X
#define X(x) #x,
#ifdef TOK_TYPES_AS_STRS
static const char *tok_type_str[] = {TOKEN_TYPES};
#endif
#undef X
#undef TOKEN_TYPES

#define NO_POS (Pos) {0}
typedef struct {
    uint32_t row;
    uint32_t col;
} Pos;

typedef struct {
    union {
        int64_t ival;
        double fval;
        bool bval;
        const char *iden;
        const char *str;
    };
    Pos pos;
    enum TokenType type;
} Token;

static inline Token token_create(enum TokenType type, void *val, Pos pos) {
    Token tok = {.type = type};
    switch (type) {
    case TT_INT:
        tok.ival = (intptr_t)val;
        break;
    case TT_FLOAT:
        tok.fval = *(double*)val;
        break;
    case TT_BOOL:
        tok.bval = (bool) val;
        break;
    case TT_IDEN:
        tok.iden = (char*) val;
        break;
    case TT_STR:
        tok.str = (char*) val;
        break;
    case TT_OP_ADD:
    case TT_OP_PRINTLN:
    default:
        break;
    }
    tok.pos = pos;
    return tok;
}

/* Lexer */

typedef struct {
    FILE *stream;
    Pos pos;
    Token *tokens;
    char current_char;
} Lexer;

bool gen_tokens(Lexer *lexer);

/* Interpreter */

#define STACK_SIZE 32

#define ERRORS                                  \
    X(ERR_NONE)                                 \
    X(ERR_STACK_OVERFLOW)                       \
    X(ERR_STACK_UNDERFLOW)                      \
    X(ERR_INVALID_TYPE)                         \
    X(ERR_UNBOUND_IDENTIFIER)                   \
    X(ERR_UNRECOGNIZED_TOKEN)
#define X(x) x,

enum Error {
ERRORS
};
#undef X
#define X(x) #x,
#ifdef ERRORS_AS_STRS
static const char *err_str[] = {ERRORS};
#endif
#undef X
#undef ERRORS

typedef struct {
    char *key;
    Token *value;
} Macro;

typedef struct {
    Token OP_STACK[STACK_SIZE];
    size_t top;
    Macro *macros;
} Interpreter;

enum Error interpreter_macro_def(Interpreter *intrprt, Token *block);

enum Error interpreter_push(Interpreter *intrprt, Token tok);
enum Error interpreter_drop(Interpreter *intrprt);

enum Error interpreter_add(Interpreter *intrprt);
enum Error interpreter_sub(Interpreter *intrprt);
enum Error interpreter_mul(Interpreter *intrprt);
enum Error interpreter_div(Interpreter *intrprt);
enum Error interpreter_mod(Interpreter *intrprt);
enum Error interpreter_gt(Interpreter *intrprt);
enum Error interpreter_gteq(Interpreter *intrprt);
enum Error interpreter_lt(Interpreter *intrprt);
enum Error interpreter_lteq(Interpreter *intrprt);
enum Error interpreter_eq(Interpreter *intrprt);
enum Error interpreter_neq(Interpreter *intrprt);
enum Error interpreter_not(Interpreter *intrprt);
enum Error interpreter_if(Interpreter *intrprt, bool *result);
enum Error interpreter_and(Interpreter *intrprt);
enum Error interpreter_or(Interpreter *intrprt);
enum Error interpreter_dup(Interpreter *intrprt);
enum Error interpreter_drop(Interpreter *intrprt);
enum Error interpreter_swap(Interpreter *intrprt);
enum Error interpreter_over(Interpreter *intrprt);
enum Error interpreter_rot(Interpreter *intrprt);

enum Error interpreter_println(Interpreter *intrprt);

void process_err(enum Error err, Token tok);

/* Parser */

typedef struct {
    Token *tokens;
    int64_t tok_idx;
    Token current_tok;
    Interpreter *intrprt;
} Parser;

static inline Token next_token(Parser *parser) {
    return ++parser->tok_idx <= stbds_arrlen(parser->tokens)
        ? parser->tokens[parser->tok_idx]
        : parser->current_tok;
}

static inline Token prev_token(Parser *parser)  {
    return --parser->tok_idx >= 0
        ? parser->tokens[parser->tok_idx]
        : parser->current_tok;
}

void parse_tokens(Parser *parser);

#endif // COCOA_H_
