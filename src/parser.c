#include "../include/stb_ds.h"
#include "../include/cocoa.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const enum TokenType BLOCK_STARTERS[] = {TT_KWRD_IF, TT_KWRD_DO};

static bool is_block_starter(enum TokenType type) {
    for (uint32_t i = 0; i < sizeof(BLOCK_STARTERS) / sizeof(BLOCK_STARTERS[0]); i++)
        if (type == BLOCK_STARTERS[i]) return true;
    return false;
}

static Token *parse_block(Parser *parser) {
    parser->current_tok = next_token(parser);
    uint64_t len, start = parser->tok_idx, starters = 1, enders = 0;
    for (len = 1; enders < starters; len++, parser->current_tok = next_token(parser)) {
        starters += is_block_starter(parser->current_tok.type) ? 1 : 0;
        enders += parser->current_tok.type == TT_TERM ? 1 : 0;
    }

    parser->current_tok = prev_token(parser);
    Token *block = NULL;
    for (uint64_t i = 0; i < len - 2; i++)
        stbds_arrput(block, parser->tokens[start + i]);

    return block;
}

void parse_tokens(Parser *parser) {
    size_t while_idx = -1;
    Token *do_block = NULL;
    parser->current_tok = next_token(parser);
    while (parser->tok_idx < stbds_arrlen(parser->tokens)) {
        /* printf("%s\n", tok_type_str[parser->current_tok.type]); */
        switch (parser->current_tok.type) {
        case TT_FLOAT:
        case TT_INT:
        case TT_BOOL:
        case TT_STR:
            process_err(
                interpreter_push(parser->intrprt, parser->current_tok),
                parser->current_tok
            );
            break;

        case TT_KWRD_MACRO:
        {
           Token *block = parse_block(parser);
            process_err(
                interpreter_macro_def(parser->intrprt, block),
                parser->current_tok
            );
        } break;
        case TT_IDEN:
        {
            if (stbds_shget(parser->intrprt->macros, (char*) parser->current_tok.iden)) {
                Parser block_parser = {
                    .tokens = stbds_shget(parser->intrprt->macros, (char*) parser->current_tok.iden),
                    .tok_idx = -1,
                    .intrprt = parser->intrprt
                };
                parse_tokens(&block_parser);
            } else {
                process_err(ERR_UNBOUND_IDENTIFIER, parser->current_tok);
            }
        } break;

        case TT_KWRD_IF:
        {
            bool _bool;
            process_err(
                interpreter_if(parser->intrprt, &_bool),
                parser->current_tok
            );

            Token *block = parse_block(parser);
            if (_bool) {
                Token *if_block = NULL;
                uint32_t if_count = 1, else_count = 0;
                for (int64_t i = 0; i < stbds_arrlen(block); i++) {
                    if (block[i].type == TT_KWRD_IF) if_count++;
                    else if (block[i].type == TT_KWRD_ELSE) else_count++;
                    if (else_count < if_count)
                        stbds_arrput(if_block, block[i]);
                    else
                        break;
                }

                Parser block_parser = {
                    .tokens = if_block,
                    .tok_idx = -1,
                    .intrprt = parser->intrprt
                };
                parse_tokens(&block_parser);
                stbds_arrfree(if_block);
            } else {
                Token *else_block = NULL;
                uint32_t if_count = 1, else_count = 0;
                for (int64_t i = 0; i < stbds_arrlen(block); i++) {
                    if (!(else_count < if_count)) {
                        stbds_arrput(else_block, block[i]);
                    }
                    if (block[i].type == TT_KWRD_IF) if_count++;
                    else if (block[i].type == TT_KWRD_ELSE) else_count++;
                }
                if (else_count == if_count) {
                    Parser block_parser = {
                        .tokens = else_block,
                        .tok_idx = -1,
                        .intrprt = parser->intrprt
                    };
                    parse_tokens(&block_parser);
                    stbds_arrfree(else_block);
                }
            }
            stbds_arrfree(block);
        } break;
        case TT_KWRD_WHILE:
            while_idx = parser->tok_idx;
            break;
        case TT_KWRD_DO:
        {
            bool _bool;
            process_err(
                interpreter_if(parser->intrprt, &_bool),
                parser->current_tok
            );

            do_block = parse_block(parser);
            if (_bool) {
                Parser block_parser = {
                    .tokens = do_block,
                    .tok_idx = -1,
                    .intrprt = parser->intrprt
                };
                parse_tokens(&block_parser);
                parser->tok_idx = while_idx;
            } else {
                while_idx = -1;
                stbds_arrfree(do_block);
            }
        } break;

        case TT_KWRD_AND:
            process_err(
                interpreter_and(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_KWRD_OR:
            process_err(
                interpreter_or(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_ADD:
            process_err(
                interpreter_add(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_SUB:
            process_err(
                interpreter_sub(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_MUL:
            process_err(
                interpreter_mul(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_DIV:
            process_err(
                interpreter_div(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_MOD:
            process_err(
                interpreter_mod(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_EQ:
            process_err(
                interpreter_eq(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_NEQ:
            process_err(
                interpreter_neq(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_GT:
            process_err(
                interpreter_gt(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_GTEQ:
            process_err(
                interpreter_gteq(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_LT:
            process_err(
                interpreter_lt(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_LTEQ:
            process_err(
                interpreter_lteq(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_OP_NOT:
            process_err(
                interpreter_not(parser->intrprt),
                parser->current_tok
            );
            break;

        case TT_KWRD_DUP:
            process_err(
                interpreter_dup(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_KWRD_DROP:
            process_err(
                interpreter_drop(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_KWRD_SWAP:
            process_err(
                interpreter_swap(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_KWRD_OVER:
            process_err(
                interpreter_over(parser->intrprt),
                parser->current_tok
            );
            break;
        case TT_KWRD_ROT:
            process_err(
                interpreter_rot(parser->intrprt),
                parser->current_tok
            );
            break;

        case TT_OP_PRINTLN:
            process_err(
                interpreter_println(parser->intrprt),
                parser->current_tok
            );
            break;
        default:
            process_err(
                ERR_UNRECOGNIZED_TOKEN,
                parser->current_tok
            );
        }
        parser->current_tok = next_token(parser);
    }
}
