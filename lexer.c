
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Types of tokens

typedef enum {
    COMPLAIN,
    PRINT,
    UNKNOWN,
    MESSAGE,
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
} Token;

// Lexer structure
typedef struct{
    char contents[100000]; // contents of the file
    char current_char; // current character we are looking at
    char* word;      // word we are extracting
    int position; // position of the current character in the file (so index)
} Lexer;

// Function to initialize a Lexer
void initializeLexer(Lexer* lexer, const char* input) {
    strcpy(lexer->contents, input);
    lexer->current_char = input[0];
    strcpy(lexer->word, "");
    lexer->position = 0;
}

// Function to skip the whitespace
void skipWhitespace(Lexer* lexer){
    while (lexer->current_char == '\n' || lexer->current_char == '\t' || lexer->current_char == ' '){
        lexer->position += 1;
        lexer->current_char = lexer->contents[lexer->position];
    }
}

// Function to get the current token
Token get_token(Lexer* lexer){
    Token token;
    token.value = NULL;
    switch(lexer->word[0]){
        case ('C'):
            if (strcmp(lexer->word, "Complain") == 0){
                token.type = COMPLAIN;
                token.value = "Complain";
                return token;
            };
            break;
        case ('P'):
            if (strcmp(lexer->word, "Print") == 0){
                token.type = PRINT;
                token.value = "Print";
                return token;
            };
            break;
        case('('):
            if (lexer->word[-1] == ')'){
                token.type = MESSAGE;
                token.value = lexer->word;
                return token;
            } else {
                //raise an error here
                fprintf(stderr, "ERROR: %s is not a valid token. Missing closing parenthesis at position %d.\n", lexer->word, lexer->position);
                exit(EXIT_FAILURE);
            };
        default:
            token.type = UNKNOWN;
            token.value = "lexer->word";
            return token;
    }
    token.type = UNKNOWN;
    token.value = lexer->word;
    return token;
}

// Function to advance the lexer to the next character
int advanceLexer(Lexer* lexer, Token* token_list, int num_tokens) {
    while (lexer->position < strlen(lexer->contents)) {
        if (lexer->current_char == ' ' || lexer->current_char == '\t' || lexer->current_char == '\n') {
            skipWhitespace(lexer);
        } else {
            char char_str[2] = {lexer->current_char, '\0'}; // get the current character
            strcat(lexer->word, char_str); // append the current character to the word
            Token this_token = get_token(lexer);
            token_list[num_tokens] = this_token;
            num_tokens++;
            lexer->position++;
            lexer->current_char = lexer->contents[lexer->position];
            lexer -> word = "";
        }
    }
    return 1;
}


int main() {
    FILE* fp;
    fp = fopen("test_file.txt", "r");
    if (fp == NULL) {
        printf("ERROR RUNNING FILE\n");
        return 1; // Return an error code
    }

    // Determine the size of the file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory for the array of characters
    char* contents = (char*)malloc(file_size + 1); // +1 for null terminator
    if (contents == NULL) {
        printf("Memory allocation error\n");
        fclose(fp);
        return 1; // Return an error code
    }

    // Read the file into the array
    fread(contents, 1, file_size, fp);
    contents[file_size] = '\0'; // Null-terminate the array

    // Create a lexer and an array of tokens
    Token* token_list = (Token*)malloc(file_size * sizeof(Token)); 
    Lexer lexer;
    int num_tokens = 0;
    initializeLexer(&lexer, contents);
    advanceLexer(&lexer,token_list, num_tokens);

    // Print or process the contents as needed
    printf("File contents:\n%s\n", contents);

    // Free the allocated memory
    free(contents);
    free(token_list);

    // Close the file
    fclose(fp);

    return 0;
}


