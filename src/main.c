#include "../include/cocoa.h"
#define STB_DS_IMPLEMENTATION
#include "../include/stb_ds.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_usage() {
    printf("Usage: cocoa <file>\n");
}

int main(int argc, char **argv) {
    // TODO: repl please

    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) return EXIT_FAILURE;
    Lexer lex = {.stream = input, .pos.row = 1};
    gen_tokens(&lex);
    Interpreter intrprt = {0};
    Parser parser = {
        .tokens = lex.tokens,
        .tok_idx = -1,
        .intrprt = &intrprt
    };
    parse_tokens(&parser);
}
