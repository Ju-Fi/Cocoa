#include <stdint.h>
#define ERRORS_AS_STRS
#define TOK_TYPES_AS_STRS
#include "../include/cocoa.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK_STACK_OVFLW() if (!(intrprt->top <= STACK_SIZE)) \
        return ERR_STACK_OVERFLOW

enum Error interpreter_push(Interpreter *intrprt, Token tok) {
    CHECK_STACK_OVFLW();

    intrprt->OP_STACK[intrprt->top++] = tok;
    return ERR_NONE;
}

static enum Error stack_pop(Interpreter *intrprt, Token *return_tok) {
    if (intrprt->top > 0) {
        intrprt->top--;
        if (return_tok)
            *return_tok = intrprt->OP_STACK[intrprt->top];

        intrprt->OP_STACK[intrprt->top] = (Token){0};
        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_add(Interpreter *intrprt) {
    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    double tmp = a.fval + b.fval;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                } else {
                    double tmp = a.fval + b.ival;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_INT, (void*) (a.ival + b.ival), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_sub(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    double tmp = a.fval - b.fval;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                } else {
                    double tmp = a.fval - b.ival;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_INT, (void*) (a.ival - b.ival), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_mul(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    double tmp = a.fval * b.fval;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                } else {
                    double tmp = a.fval * b.ival;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_INT, (void*) (a.ival * b.ival), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_div(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    double tmp = a.fval / b.fval;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                } else {
                    double tmp = a.fval / b.ival;
                    interpreter_push(intrprt, token_create(TT_FLOAT, &tmp, NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_INT, (void*) (a.ival / b.ival), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_mod(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT) && (b.type == TT_INT )) {
            interpreter_push(intrprt, token_create(TT_INT, (void*) (a.ival % b.ival), NO_POS));
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_not(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 1) {
        Token a;
        enum Error err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if (a.type == TT_BOOL) {
            interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) !a.bval), NO_POS));
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_gt(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval > b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval > b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival > b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_gteq(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval >= b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval >= b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival >= b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_lt(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval < b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval < b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival < b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_lteq(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval <= b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval <= b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival <= b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_eq(Interpreter *intrprt) {
    CHECK_STACK_OVFLW();

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval == b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval == b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival == b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_neq(Interpreter *intrprt) {
    CHECK_STACK_OVFLW(); // TODO Fix stack overflow errors (wrong location)

    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if ((a.type == TT_INT || a.type == TT_FLOAT)
            && (b.type == TT_INT || b.type == TT_FLOAT)) {

            if (a.type == TT_FLOAT) {
                if (b.type == TT_FLOAT) {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval != b.fval)), NO_POS));
                } else {
                    interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.fval != b.ival)), NO_POS));
                }
            } else {
                interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival != b.ival)), NO_POS));
            }
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_if(Interpreter *intrprt, bool *result) {
    if (intrprt->top >= 1) {
        Token a;
        enum Error err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        if (a.type == TT_BOOL) {
            *result = a.bval;
            return ERR_NONE;
        }
        return ERR_INVALID_TYPE;
    }
    return ERR_STACK_UNDERFLOW;
}

#define GEN_LOGIC_OP(postfix, op)                   \
    enum Error interpreter_##postfix(Interpreter *intrprt) { \
    if (intrprt->top >= 2) { \
        Token a, b; \
        enum Error err = stack_pop(intrprt, &b); \
        if (err != ERR_NONE) return err; \
        err = stack_pop(intrprt, &a); \
        if (err != ERR_NONE) return err; \
        if ((a.type == TT_BOOL) && (b.type == TT_BOOL )) { \
        interpreter_push(intrprt, token_create(TT_BOOL, (void*) ((bool) (a.ival op b.ival)), NO_POS)); \
            return ERR_NONE; \
        } \
        return ERR_INVALID_TYPE; \
    } \
    return ERR_STACK_UNDERFLOW; \
}

GEN_LOGIC_OP(and, &&)
GEN_LOGIC_OP(or, ||);

enum Error interpreter_dup(Interpreter *intrprt) {
    if (intrprt->top >= 1) {
        Token a = intrprt->OP_STACK[intrprt->top - 1];

        interpreter_push(intrprt, a);
        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_drop(Interpreter *intrprt) {
    if (intrprt->top >= 1) {
        stack_pop(intrprt, NULL);
        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_swap(Interpreter *intrprt) {
    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        interpreter_push(intrprt, b);
        interpreter_push(intrprt, a);

        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_over(Interpreter *intrprt) {
    if (intrprt->top >= 2) {
        Token a, b;
        enum Error err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        interpreter_push(intrprt, a);
        interpreter_push(intrprt, b);
        interpreter_push(intrprt, a);

        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_rot(Interpreter *intrprt) {
    if (intrprt->top >= 3) {
        Token a, b, c;
        enum Error err = stack_pop(intrprt, &c);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &b);
        if (err != ERR_NONE) return err;
        err = stack_pop(intrprt, &a);
        if (err != ERR_NONE) return err;

        // c b a -> (a b c)

        interpreter_push(intrprt, c);
        interpreter_push(intrprt, a);
        interpreter_push(intrprt, b);

        return ERR_NONE;
    }
    return ERR_STACK_UNDERFLOW;
}

enum Error interpreter_macro_def(Interpreter *intrprt, Token *block) {
    if (block[0].type != TT_IDEN) return ERR_INVALID_TYPE;
    char *iden = (char*) block[0].iden;

    Token *new_block = NULL;
    for (int64_t i = 1; i < stbds_arrlen(block); i++)
        stbds_arrput(new_block, block[i]);
    stbds_arrfree(block);

    stbds_shput(intrprt->macros, iden, new_block);
    return ERR_NONE;
}

// TODO Temporary function for debugging purposes
enum Error interpreter_println(Interpreter *intrprt) {
    Token a;
    enum Error err = stack_pop(intrprt, &a);
    if (err != ERR_NONE)
        return err;

    switch (a.type) {
    case TT_FLOAT:
        printf("%lf\n", a.fval);
        break;
    case TT_INT:
        printf("%ld\n", a.ival);
        break;
    case TT_BOOL:
        printf("%s\n", a.bval ? "true" : "false");
        break;
    case TT_NIL:
        printf("(nil)\n");
        break;
    case TT_STR:
        printf("%s\n", a.str);
        break;
    default:
        return ERR_INVALID_TYPE;
    }
    return ERR_NONE;
}

void process_err(enum Error err, Token tok) {
    if (err == ERR_NONE) return;
    fprintf(stderr, "[ERROR] <TODO: File name>:%d:%d: %s: <TODO: better error messages> %s\n", tok.pos.row, tok.pos.col,
            err_str[err],
            tok_type_str[tok.type]);
    exit(EXIT_FAILURE);
}
