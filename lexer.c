#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Types of tokens
typedef enum {
    PRINT,
    SET,
    ADD,
    IFZERO,
    COMPLAIN,
    UNKNOWN,
    MESSAGE,
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
} Token;

// Lexer structure
typedef struct {
    char contents[100000];
    char current_char;
    char* word;
    int position;
} Lexer;

// Function to initialize a Lexer
void initializeLexer(Lexer* lexer, const char* input) {
    strcpy(lexer->contents, input);
    lexer->current_char = input[0];
    lexer->word = malloc(sizeof(char) * 1000);
    lexer->word[0] = '\0';
    lexer->position = 0;
}

// Function to skip the whitespace
void skipWhitespace(Lexer* lexer) {
    while (lexer->current_char == '\n' || lexer->current_char == '\t' || lexer->current_char == ' ') {
        lexer->position += 1;
        lexer->current_char = lexer->contents[lexer->position];
    }
}

// Function to get the current token
Token get_token(Lexer* lexer) {
    Token token;
    token.value = NULL;
    // Check for COMPLAIN token
    switch (lexer->word[0]) {
        case 'P':
            if (strcmp(lexer->word, "PRINT") == 0) {
                token.type = PRINT;
                token.value = strdup("Print");
            }
            break;
        case 'S':
            if (strcmp(lexer->word, "SET") == 0) {
                token.type = SET;
                token.value = strdup("Set");
            }
            break;
        case 'A':
            if (strcmp(lexer->word, "ADD") == 0) {
                token.type = ADD;
                token.value = strdup("Add");
            }
            break;
        case 'I':
            if (strcmp(lexer->word, "IFZERO") == 0) {
                token.type = IFZERO;
                token.value = strdup("IfZero");
            }
            break;
        case 'C':
            if (strcmp(lexer->word, "COMPLAIN") == 0) {
                token.type = COMPLAIN;
                token.value = strdup("Complain");
            }
            break;
        default:
            token.type = UNKNOWN;
            token.value = strdup(lexer->word);
            break;
        }
    return token;
}

// Function to advance the lexer to the next character
int advanceLexer(Lexer* lexer, Token** token_list, int* num_tokens) {
    while (lexer->position < strlen(lexer->contents)) {
        if (lexer->current_char == ' ' || lexer->current_char == '\t' || lexer->current_char == '\n') {
            skipWhitespace(lexer);
        } else if (lexer->current_char == '(') {
            // Reset the word for the next token
            strcpy(lexer->word, "");

            // Skip the '(' character
            lexer->position++;
            lexer->current_char = lexer->contents[lexer->position];

            // Read everything until ')'
            while (lexer->current_char != ')' && lexer->current_char != '\0') {
                char char_str[2] = {lexer->current_char, '\0'};
                strcat(lexer->word, char_str);
                lexer->position++;
                lexer->current_char = lexer->contents[lexer->position];
            }

            // Include the ')' in the word
            if (lexer->current_char == ')') {
                Token this_token;
                this_token.type = MESSAGE;
                this_token.value = strdup(lexer->word);
                (*token_list)[*num_tokens] = this_token;
                (*num_tokens)++;

                // Move to the next character after ')'
                lexer->position++;
                lexer->current_char = lexer->contents[lexer->position];
            } else {
                printf("ERROR: Expected ')' but found EOF\n");
                return 0;
            }

        } else {
            // Reset the word for the next token
            strcpy(lexer->word, "");

            // Read everything until whitespace or special character
            while (lexer->current_char != ' ' && lexer->current_char != '\t' && lexer->current_char != '\n') {
                char char_str[2] = {lexer->current_char, '\0'};
                strcat(lexer->word, char_str);
                lexer->position++;
                if (lexer->position >= strlen(lexer->contents)) {
                    // Reached the end of the file
                    break;
                }
                lexer->current_char = lexer->contents[lexer->position];
            }
            // Create a token based on the word
            Token this_token = get_token(lexer);
            (*token_list)[*num_tokens] = this_token;
            (*num_tokens)++;
        }
    }
    return 1;
}

void perform_complain() {
    printf("Complain: This is a generic complaint.\n");
}

void perform_print(const char* value) {
    printf("Printing message: %s\n", value);
}

void perform_set(const char* variable, const char* value) {
    printf("Set: %s = %s\n", variable, value);
}

void perform_add(int var1,int var2) {
    printf("Add: %d + %d = %d\n", var1, var2, var1+var2);
}

void perform_ifzero(int variable) {
    if (variable == 0) {
        printf("IfZero: %d is zero.\n", variable);
    } else {
        printf("IfZero: %d is not zero.\n", variable);
    }
}

int main() {
    FILE* fp;
    fp = fopen("test_file.txt", "r");
    if (fp == NULL) {
        printf("ERROR RUNNING FILE\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* contents = (char*)calloc(file_size, sizeof(char));
    if (contents == NULL) {
        printf("Memory allocation error for contents\n");
        fclose(fp);
        return 1;
    }

    // Read file contents into the 'contents' buffer
    fread(contents, sizeof(char), file_size, fp);

    Token* token_list = (Token*)malloc(file_size * sizeof(Token));
    if (token_list == NULL) {
        printf("Memory allocation error for tokens\n");
        free(contents);
        fclose(fp);
        return 1;
    }

    Lexer lexer;
    int num_tokens = 0;
    initializeLexer(&lexer, contents);
    advanceLexer(&lexer, &token_list, &num_tokens);
    for (int i = 0; i < num_tokens; ++i) {
        Token token = token_list[i];
        switch (token.type) {
            case COMPLAIN:
                perform_complain();
                break;
            case PRINT:
                if (i + 1 < num_tokens && token_list[i + 1].type == MESSAGE) {
                    perform_print(token_list[i + 1].value);
                    i++;
                }
                break;
            case SET:
                if (i + 1 < num_tokens && token_list[i + 1].type == MESSAGE) {
                    char* variable_pair = strtok(token_list[i + 1].value, " ");

                    // Assume there are exactly two words
                    char* variable = variable_pair;
                    char* value = strtok(NULL, " ");

                    // Remove commas from the words
                    char* comma = strchr(variable, ',');
                    if (comma != NULL) {
                        *comma = '\0'; // Replace the comma with null character
                    }

                    comma = strchr(value, ',');
                    if (comma != NULL) {
                        *comma = '\0'; // Replace the comma with null character
                    }

                    // Call the function with the separated and comma-removed words
                    perform_set(variable, value);
                    i++;
                }
                break;
            case ADD:
                if (i + 1 < num_tokens && token_list[i + 1].type == MESSAGE) {
                    char* variable_pair = strtok(token_list[i + 1].value, " ");

                    // Assume there are exactly two words
                    char* value1_str = variable_pair;
                    char* value2_str = strtok(NULL, " ");

                    // Remove commas from the words
                    char* comma = strchr(value1_str, ',');
                    if (comma != NULL) {
                        *comma = '\0'; // Replace the comma with null character
                    }

                    comma = strchr(value2_str, ',');
                    if (comma != NULL) {
                        *comma = '\0'; // Replace the comma with null character
                    }

                    int value1 = atoi(value1_str);
                    int value2 = atoi(value2_str);

                    // Call the function with the separated and comma-removed words
                    perform_add(value1, value2);
                    i++;
                }
                break;
            case IFZERO:
                if (i + 1 < num_tokens && token_list[i + 1].type == MESSAGE) {
                    char *endptr; // To check for extra characters in the conversion
                    int number = strtol(token_list[i + 1].value, &endptr, 10);

                    // Check for conversion errors
                    if (*endptr != '\0' && *endptr != '\n') {
                        printf("Conversion failed. Not a valid integer.\n");
                    } else {
                        // The conversion was successful
                        printf("Converted number: %d\n", number);

                        // Now you can check if the number is within a valid range, for example
                        if (number >= 0 && number <= 100) {
                            perform_ifzero(number);
                        } else {
                            printf("Not within the valid range.\n");
                        }
                 }
                    i++;
                }
                break;
            case MESSAGE:
                break;
            default:
                printf("Unknown token encountered.\n");
                break;
        }
    }

    // Free the memory
    free(lexer.word);
    free(contents);
    free(token_list);
    fclose(fp);

    return 0;
}
