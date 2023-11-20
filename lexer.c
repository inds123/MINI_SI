#include "src/lexer.h"
#include <stdlib.h>
#include <string.h>


Lexer* init_lexer(char* contents){
    Lexer* lexer = (Lexer*)calloc(1, sizeof(Lexer));
    lexer->contents = contents;
    lexer->i = 0; // we set at the beginning of the file
    lexer->c = contents[lexer->i]; // we set the first character of the file
    return lexer;
}

// Function that moves our lexer pointer forward.
void lexer_advance(Lexer* lexer){
    if(lexer->c != '\0' && lexer->i < strlen(lexer->contents)){
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
}

// Function that skips the whitespace or new line.
void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->c == ' ' || lexer->c == '\n') {
        lexer_advance(lexer);
    }
}

// Function to extract the next token from the input string being processed by the lexer.
// Actually when we write code, we write it in text format/strings that then need to be processed hence why this step
Token* lexer_get_next_token(Lexer* lexer){
    while (lexer->c != '\0' && lexer->i < strlen(lexer->contents)){
        lexer_skip_whitespace(lexer);
    }
    // write more here
}


//
Token* lexer_collect_string(Lexer* lexer);

Token* lexer_collect_id(Lexer* lexer);

Token* lexer_advance_with_token(Lexer* lexer, Token* token);

// Function to turn character into string
char* lexer_get_current_char_as_string(Lexer* lexer);
