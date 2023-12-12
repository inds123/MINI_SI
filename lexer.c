#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Names of tokens
typedef enum {
    PRINT,
    SET,
    ADD,
    ISZERO, // (ADLT) Instead of IFZERO, it would be clearer to name it ISZERO or ISZEROCHECK
    COMPLAIN,
    UNKNOWN,
    MESSAGE,
    ARGUMENT
} TokenName;

typedef enum {
    COMMAND,
    MESSAGE_TYPE,
    ARGUMENT_TYPE,
    UNKNOWN_TYPE
} TokenType;

typedef enum {
    ROOT,
    CHILD
} TreeNodeType;

// Token structure
typedef struct {
    TokenName name;
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
    lexer->current_char = input[0]; // (ADLT) Why not lexer->contents[0]?
    lexer->word = malloc(sizeof(char) * 1000);
    lexer->word[0] = '\0';
    lexer->position = 0;
}

// Function to skip the whitespace
void skipWhitespace(Lexer* lexer) { // (ADLT) Change function name to skip_whitespace_tab_newline()
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
    // (ADLT) checking for the multiple tokens not just "COMPLAIN"
    // (ADLT) We assume that each command starts with a different letter or no 2 commands start with the same letter
    switch (lexer->word[0]) {
        case 'P':
            if (strcmp(lexer->word, "PRINT") == 0) {
                token.name = PRINT;
                token.type = COMMAND;
                token.value = strdup("Print");
            }
            break;
        case 'S':
            if (strcmp(lexer->word, "SET") == 0) {
                token.name = SET;
                token.type = COMMAND;
                token.value = strdup("Set");
            }
            break;
        case 'A':
            if (strcmp(lexer->word, "ADD") == 0) {
                token.name = ADD;
                token.type = COMMAND;
                token.value = strdup("Add");
            }
            break;
        case 'I':
            if (strcmp(lexer->word, "ISZERO") == 0) {
                token.name = ISZERO;
                token.type = COMMAND;
                token.value = strdup("IsZero");
            }
            break;
        case 'C':
            if (strcmp(lexer->word, "COMPLAIN") == 0) {
                token.name = COMPLAIN;
                token.type = COMMAND;
                token.value = strdup("Complain");
            }
            break;
        default:
            token.name = UNKNOWN;
            token.type = UNKNOWN_TYPE;
            token.value = strdup(lexer->word);
            break;
        }
    return token;
}

// AST Node structure
typedef struct ASTNode {
    Token token;
    TreeNodeType type;
    struct ASTNode* nextNode;
    struct ASTNode** childNodes; // Array of pointers to child nodes
    int numChildNodes; // Number of child nodes
} ASTNode;

// Function to create a new AST Node
ASTNode* newASTNode(Token token, TreeNodeType nodeType) {
    ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
    node->token = token;
    node->type = nodeType;
    node->nextNode = NULL;
    node->childNodes = NULL; // Initially, there are no child nodes
    node->numChildNodes = 0; // Initially, there are no child nodes
    return node;
}

// Function to add a child node to a parent node
void addChildNode(ASTNode* parentNode, ASTNode* childNode) {
    parentNode->numChildNodes++;
    parentNode->childNodes = realloc(parentNode->childNodes, parentNode->numChildNodes * sizeof(ASTNode*));
    parentNode->childNodes[parentNode->numChildNodes - 1] = childNode;
}

// Function to print the AST tree
void printAST(ASTNode* node, int depth) {

    // Array of token names
    char* tokenNames[] = {
        "PRINT",
        "SET",
        "ADD",
        "ISZERO",
        "COMPLAIN",
        "UNKNOWN",
        "MESSAGE",
        "ARGUMENT"  // Added ARGUMENT
    };

    // Array of token types
    char* tokenTypes[] = {
        "COMMAND",
        "MESSAGE_TYPE",
        "ARGUMENT_TYPE",
        "UNKNOWN_TYPE"
        
    };

    // Array of tree node types
    char* treeNodeTypes[] = {
        "ROOT",
        "CHILD"
        // Add more node types as needed
    };

    // Print the current node
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("Node Type: %s, Token Name: %s, Token Type: %s, Value: %s\n", 
           treeNodeTypes[node->type],
           tokenNames[node->token.name], 
           tokenTypes[node->token.type], 
           node->token.value);

    // Print all child nodes
    for (int i = 0; i < node->numChildNodes; i++) {
        printAST(node->childNodes[i], depth + 1);
    }
}

// Function to advance the lexer to the next character
// (ADLT) Advance to the next character or the next word?
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
            // Is the comment above correct? I dont think the ')' is being included in the word but it is creating a message token
            if (lexer->current_char == ')') {
                Token this_token;
                this_token.name = MESSAGE;
                this_token.type = MESSAGE_TYPE;
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

// Function to parse tokens into an AST
ASTNode* parseTokensIntoAST(Token* token_list, int num_tokens) {
    // Create the root node
    ASTNode* root = newASTNode((Token){UNKNOWN, UNKNOWN_TYPE, NULL}, ROOT);

    // Keep track of the current parent node
    ASTNode* currentParent = root;

    for (int i = 0; i < num_tokens; ++i) {
        // Depending on the type of the token, add it to the AST
        switch (token_list[i].type) {
            case COMMAND:
                // Create a new node for the current token
                ASTNode* newNode = newASTNode(token_list[i], CHILD);
                // Add the new node as a child of the root
                addChildNode(root, newNode);
                // The new node is now the current parent for ARGUMENT and MESSAGE_TYPE tokens
                currentParent = newNode;
                break;
            case ARGUMENT:
            case MESSAGE_TYPE:
                // If the token value contains a comma, split it into multiple tokens
                if (strchr(token_list[i].value, ',') != NULL) {
                    char* value = strtok(token_list[i].value, ",");
                    while (value != NULL) {
                        Token newToken = {ARGUMENT, ARGUMENT_TYPE, value};
                        ASTNode* newNode = newASTNode(newToken, CHILD);
                        addChildNode(currentParent, newNode);
                        value = strtok(NULL, ",");
                    }
                } else {
                    // Create a new token with the correct type
                    Token newToken = {ARGUMENT, ARGUMENT_TYPE, token_list[i].value};
                    // Create a new node for the new token
                    ASTNode* newNode = newASTNode(newToken, CHILD);
                    // Add the new node as a child of the current parent
                    addChildNode(currentParent, newNode);
                }
                break;
            default:
                // For unknown types, just add the new node as a child of the root
                ASTNode* newNodeChild = newASTNode(token_list[i], CHILD);
                addChildNode(root, newNodeChild);
                break;
        }
    }

    return root;
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

void perform_iszero(int variable) {
    if (variable == 0) {
        printf("IsZero: %d is zero.\n", variable);
    } else {
        printf("IsZero: %d is not zero.\n", variable);
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
    // Parse the file and get the list of tokens and the number of tokens
    advanceLexer(&lexer, &token_list, &num_tokens); // (ADLT) Chnage name of "advanceLexer()" to "ParseWithLexer()"

    // Generate the AST
    ASTNode* root = parseTokensIntoAST(token_list, num_tokens);

    // Print the AST tree
    printAST(root, 0);

    // Run/execute the tokens that are commands (for instance a "MESSAGE" is a token, but not a command)
    for (int i = 0; i < num_tokens; ++i) {
        Token token = token_list[i];
        switch (token.name) {
            case COMPLAIN:
                perform_complain();
                break;
            case PRINT:
                if (i + 1 < num_tokens && token_list[i + 1].name == MESSAGE) {
                    perform_print(token_list[i + 1].value);
                    i++;
                }
                break;
            case SET:
                if (i + 1 < num_tokens && token_list[i + 1].name == MESSAGE) {
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
                if (i + 1 < num_tokens && token_list[i + 1].name == MESSAGE) {
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
            case ISZERO:
                if (i + 1 < num_tokens && token_list[i + 1].name == MESSAGE) {
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
                            perform_iszero(number);
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