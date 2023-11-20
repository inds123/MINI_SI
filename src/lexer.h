#include "token.h"
#ifndef FINAL_PROJECT_LEXER_H
#define FINAL_PROJECT_LEXER_H

#endif //FINAL_PROJECT_LEXER_H


typedef struct LEXER_STRUCT
{
    char c;
    unsigned int i;
    char* contents;
} Lexer;


// Function that initializes the lexer.
// takes the contents of the file as a parameter and returns a pointer to the lexer structure.
Lexer* init_lexer(char* contents);

// Function that moves our lexer forward.
void lexer_advance(Lexer* lexer);

// Function that skips the whitespace.
void lexer_skip_whitespace(Lexer* lexer);

// Function that skips the comments.
Token* lexer_get_next_token(Lexer* lexer);

// Function that helps us parse
Token* lexer_collect_string(Lexer* lexer);

Token* lexer_collect_id(Lexer* lexer);

Token* lexer_advance_with_token(Lexer* lexer, Token* token);

char* lexer_get_current_char_as_string(Lexer* lexer);


