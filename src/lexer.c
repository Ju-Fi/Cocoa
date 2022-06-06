#include "../include/cocoa.h"
#include "../include/stb_ds.h"
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool is_alphanum(char c) {
        return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_' || is_digit(c);
}

static inline char advance(Lexer *lexer) {
    char c = fgetc(lexer->stream);
    if (c == '\n') {
        lexer->pos.row++;
        lexer->pos.col = 0;
    }
    lexer->pos.col++;
    return c;
}

Token make_num(Lexer *lexer) {
    bool is_float = false;
    int32_t fpoint_loc = 0;
    uint32_t width = 0;

    int64_t num = atoi(&lexer->current_char);
    for (uint32_t i = 1; is_digit(lexer->current_char); i++) {
        lexer->current_char = advance(lexer);
        if (lexer->current_char == '.') {
            is_float = true;
            lexer->current_char = advance(lexer);
            if (!is_digit(lexer->current_char)) {
                i--;
                continue;
            }
        }
        width++;
        if (is_float) fpoint_loc++;
        num += atoi(&lexer->current_char) * powl(10, i);
    }

    int64_t reverse = 0, remainder;
    while (width-- > 0) {
        remainder = num % 10;
        reverse = reverse * 10 + remainder;
        num /= 10;
    }

    double reverse_float = reverse * pow(10, -fpoint_loc + 1);

    Token result = token_create(is_float ? TT_FLOAT : TT_INT, is_float
                                ? (void*) &reverse_float
                                : (void*)reverse,
                                lexer->pos);
    return result;
}

#define MAX_WORD_LEN 80
Token make_word(Lexer *lexer) {
    char word_buf[MAX_WORD_LEN];
    uint32_t len;
    for (len = 0; is_alphanum(lexer->current_char); len++,
             lexer->current_char = advance(lexer))
        word_buf[len] = tolower(lexer->current_char);

    char *word = malloc(len + 1);
    strncpy(word, word_buf, len);
    word[len] = '\0';

    return strncmp("true", word, len) == 0
        ? token_create(TT_BOOL, (void*) true, lexer->pos)
        : strncmp("false", word, len) == 0
        ? token_create(TT_BOOL, (void*) false, lexer->pos)
        : strncmp("println", word, len) == 0
        ? token_create(TT_OP_PRINTLN, NULL, lexer->pos)
        : strncmp("if", word, len) == 0
        ? token_create(TT_KWRD_IF, NULL, lexer->pos)
        : strncmp("else", word, len) == 0
        ? token_create(TT_KWRD_ELSE, NULL, lexer->pos)
        : strncmp("while", word, len) == 0
        ? token_create(TT_KWRD_WHILE, NULL, lexer->pos)
        : strncmp("do", word, len) == 0
        ? token_create(TT_KWRD_DO, NULL, lexer->pos)
        : strncmp("or", word, len) == 0
        ? token_create(TT_KWRD_OR, NULL, lexer->pos)
        : strncmp("and", word, len) == 0
        ? token_create(TT_KWRD_AND, NULL, lexer->pos)
        : strncmp("dup", word, len) == 0
        ? token_create(TT_KWRD_DUP, NULL, lexer->pos)
        : strncmp("drop", word, len) == 0
        ? token_create(TT_KWRD_DROP, NULL, lexer->pos)
        : strncmp("swap", word, len) == 0
        ? token_create(TT_KWRD_SWAP, NULL, lexer->pos)
        : strncmp("over", word, len) == 0
        ? token_create(TT_KWRD_OVER, NULL, lexer->pos)
        : strncmp("rot", word, len) == 0
        ? token_create(TT_KWRD_ROT, NULL, lexer->pos)
        : strncmp("macro", word, len) == 0
        ? token_create(TT_KWRD_MACRO, NULL, lexer->pos)

        : strncmp("end", word, len) == 0
        ? token_create(TT_TERM, NULL, lexer->pos)

        : token_create(TT_IDEN, word, lexer->pos);
}

Token make_str(Lexer *lexer) {
    char *word_buf = NULL;
    while (lexer->current_char != '"') {
        stbds_arrput(word_buf, lexer->current_char);
        lexer->current_char = advance(lexer);
    }
    lexer->current_char = advance(lexer);

    size_t len = stbds_arrlenu(word_buf);
    char *str = malloc(len + 1);
    for (size_t i = 0; i < len; i++)
        str[i] = word_buf[i];
    str[len] = '\0';

    return token_create(TT_STR, (void*) str, lexer->pos);
}

bool gen_tokens(Lexer *lexer) {
    /* lexer->tokens = NULL; */

    lexer->current_char = advance(lexer);
    bool error = false;

    while (lexer->current_char != EOF && lexer->current_char != (lexer->stream == stdin ? '\n' : '\0') && !error) {
        switch (lexer->current_char) {
        case ' ':
        case '\t':
        case '\n':
            lexer->current_char = advance(lexer);
            break;
        case ';':
            stbds_arrput(lexer->tokens, token_create(TT_TERM, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case'+':
            stbds_arrput(lexer->tokens, token_create(TT_OP_ADD, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '-':
            stbds_arrput(lexer->tokens, token_create(TT_OP_SUB, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '*':
            stbds_arrput(lexer->tokens, token_create(TT_OP_MUL, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '/':
            stbds_arrput(lexer->tokens, token_create(TT_OP_DIV, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '%':
            stbds_arrput(lexer->tokens, token_create(TT_OP_MOD, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '>':
            lexer->current_char = advance(lexer);
            stbds_arrput(lexer->tokens, token_create(lexer->current_char == '='
                                                     ? TT_OP_GTEQ : TT_OP_GT, NULL, lexer->pos));
            break;
        case '<':
            lexer->current_char = advance(lexer);
            stbds_arrput(lexer->tokens, token_create(lexer->current_char == '='
                                                     ? TT_OP_LTEQ : TT_OP_LT, NULL, lexer->pos));
            break;
        case '=':
            stbds_arrput(lexer->tokens, token_create(TT_OP_EQ, NULL, lexer->pos));
            lexer->current_char = advance(lexer);
            break;
        case '!':
            lexer->current_char = advance(lexer);
            stbds_arrput(lexer->tokens, token_create(lexer->current_char == '='
                                                     ? TT_OP_NEQ : TT_OP_NOT, NULL, lexer->pos));
            break;
        case '"':
            lexer->current_char = advance(lexer);
            stbds_arrput(lexer->tokens, make_str(lexer));
            break;

        case '(':
            while ((lexer->current_char = advance(lexer)) != ')') {
                if (lexer->current_char == EOF) {
                    fprintf(stderr, "[ERROR] <TODO FIle name>:%d:%d: Invalid comment\n", lexer->pos.row, lexer->pos.col);
                    exit(EXIT_FAILURE);
                }
            }
            lexer->current_char = advance(lexer);
            break;

        default:
            if (is_digit(lexer->current_char))
                stbds_arrput(lexer->tokens, make_num(lexer));
            else if (is_alphanum(lexer->current_char))
                stbds_arrput(lexer->tokens, make_word(lexer));
            else {
                fprintf(stderr, "[ERROR] <TODO File name>:%d:%d: Invalid character '%c' inputted\n",
                        lexer->pos.row, lexer->pos.col, lexer->current_char);
                exit(EXIT_FAILURE);
                // error = true;
            }
        }
    }
    return error;
}
