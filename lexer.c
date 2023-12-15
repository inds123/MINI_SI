#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Names of tokens
typedef enum {
    PRINT,
    SET,
    ADD,
    ISEQUAL,
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

// Define token names globally
char* tokenNames[] = {
        "PRINT",
        "SET",
        "ADD",
        "ISEQUAL",
        "COMPLAIN",
        "UNKNOWN",
        "MESSAGE",
        "ARGUMENT"
};

typedef enum {
    ROOT,
    CHILD,
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
    lexer->current_char = input[0];
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
    // Check the first character of the word for the switch statement then compare the rest
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
            if (strcmp(lexer->word, "ISEQUAL") == 0) {
                token.name = ISEQUAL;
                token.type = COMMAND;
                token.value = strdup("IsEqual");
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
typedef struct {
    Token token;
    TreeNodeType type;
    struct ASTNode* nextNode;
    struct ASTNode** childNodes; // Array of pointers to child nodes
    int numChildNodes; // Number of child nodes
} ASTNode;

// Function to create a new AST Node
ASTNode* newASTNode(Token token, TreeNodeType nodeType) {
    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
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
    if (parentNode->childNodes == NULL) {
        printf("Memory allocation error for child nodes\n");
        exit(1);
    }
    parentNode->childNodes[parentNode->numChildNodes - 1] = childNode;
}

// Function to print the AST tree
void printAST(ASTNode* node, int depth) {
    // Array of token names
    char* tokenNames[] = {
        "PRINT",
        "SET",
        "ADD",
        "ISEQUAL",
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
    char* NodeTypes[] = {
            "ROOT",
            "CHILD"
        // Add more node types as needed
    };

    // Print the current node
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    printf("Node Type: %s, Token Name: %s, Token Type: %s, Value: %s\n",
           NodeTypes[node->type],
           tokenNames[node->token.name],
           tokenTypes[node->token.type],
           node->token.value);

    // Print all child nodes
    for (int i = 0; i < node->numChildNodes; i++) {
        printAST(node->childNodes[i], depth + 1);
    }
}

// Function to advance the lexer to the next word
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

            // if string
            if (lexer->current_char == '"') {
                lexer->position++;
                lexer->current_char = lexer->contents[lexer->position];

                while (lexer->current_char != '"' && lexer->current_char != '\0') {
                    char char_str[2] = {lexer->current_char, '\0'};
                    strcat(lexer->word, char_str);
                    lexer->position++;
                    lexer->current_char = lexer->contents[lexer->position];
                }

                int position_curr = lexer->position;
                position_curr++;
                char next_char = lexer->contents[position_curr];


                if (lexer->current_char == '"' && next_char == ')') {
                    Token this_token;
                    this_token.name = MESSAGE;
                    this_token.type = MESSAGE_TYPE;
                    this_token.value = strdup(lexer->word);
                    (*token_list)[*num_tokens] = this_token;
                    (*num_tokens)++;

                    // Move to the next character after '"'
                    lexer->position++;
                    lexer->position++;
                    lexer->current_char = lexer->contents[lexer->position];

                } else {
                    printf("ERROR: Expected """ "but found EOF\n");
                    return 0;
                }

            } else {
                // Read everything until ')'
                while (lexer->current_char != ')' && lexer->current_char != '\0') {
                    char char_str[2] = {lexer->current_char, '\0'};
                    strcat(lexer->word, char_str);
                    lexer->position++;
                    lexer->current_char = lexer->contents[lexer->position];
                }

                // Find ")"
                if (lexer->current_char == ')') {
                    Token this_token;
                    this_token.name = ARGUMENT;
                    this_token.type = ARGUMENT_TYPE;
                    this_token.value = strdup(lexer->word);
                    (*token_list)[*num_tokens] = this_token;
                    (*num_tokens)++;

                    // Move to the next character after ')'
                    lexer->position++;
                    lexer->position++;
                    lexer->current_char = lexer->contents[lexer->position];

                } else {
                    printf("ERROR: Expected ')' but found EOF\n");
                    return 0;
                }
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
    Token root_token = {UNKNOWN, UNKNOWN_TYPE, NULL};
    ASTNode* root = newASTNode(root_token, ROOT);

    // Keep track of the current parent node
    ASTNode* currentParent = root;

    ASTNode *newNode;
    for (int i = 0; i < num_tokens; ++i) {
        // Depending on the type of the token, add it to the AST
        switch (token_list[i].type) {
            case COMMAND:
                // Create a new node for the current token
                newNode = newASTNode(token_list[i], CHILD);
                // Add the new node as a child of the root
                addChildNode(root, newNode);
                // The new node is now the current parent for ARGUMENT and MESSAGE_TYPE tokens
                currentParent = newNode;
                break;
            case MESSAGE_TYPE:
                // Create a new node for the new token
                newNode = newASTNode(token_list[i], CHILD);
                // Add the new node as a child of the current parent
                addChildNode(currentParent, newNode);
                break;
            case ARGUMENT_TYPE:
                // If the token value contains a comma, split it into multiple tokens
                if (strchr(token_list[i].value, ',') != NULL) {
                    char* value = strtok(token_list[i].value, ",");
                    size_t numTokens = sizeof(tokenNames) / sizeof(tokenNames[0]);
                    for (int j = 0; j < numTokens; j++) {
                        if (strcmp(value, tokenNames[j]) == 0) {
                            break;
                        }
                    }
                    while (value != NULL) {
                        Token newToken = {ARGUMENT, ARGUMENT_TYPE, value};
                        newNode = newASTNode(newToken, CHILD);
                        addChildNode(currentParent, newNode);
                        value = strtok(NULL, ",");
                    }
                } else {
                    // Create a new token with the correct type
                    Token newToken = {ARGUMENT, ARGUMENT_TYPE, token_list[i].value};
                    // Create a new node for the new token
                    newNode = newASTNode(newToken, CHILD);
                    // Add the new node as a child of the current parent
                    addChildNode(currentParent, newNode);
                }
                break;
            default:
                // For unknown types, just add the new node as a child of the root
                newNode = newASTNode(token_list[i], CHILD);
                addChildNode(root, newNode);
                break;
        }
    }
    return root;
}

// AST Interpreter

void perform_complain();
void perform_print(char* arguments[]);
void perform_set(char* arguments[]);
void perform_add(char* arguments[]);
void perform_isequal(char* arguments[]);
void freeAST(ASTNode* root);


// Function to execute the commands
void processNode(ASTNode* root, char* arguments[]){
    switch (root->token.name) {
        case PRINT:
            perform_print(arguments);
            break;
        case SET:
            perform_set(arguments);
            break;
        case ADD:
            perform_add(arguments);
            break;
        case ISEQUAL:
            perform_isequal(arguments);
            break;
        case COMPLAIN:
            perform_complain();
            break;
        default:
            printf("Error, unknown syntax.\n");
            break;
    }
}


// Function to transverse the AST
void interpreter(ASTNode* root){
    if (root == NULL) {
        return;
    }

    printf("root: %s\n", root->token.value);

    if (root->type == ROOT) {
        for (int i = 0; i < root->numChildNodes; i++) {
            ASTNode* child = root->childNodes[i];
            printf("child: %s\n", child->token.value);
            char * arguments[2] = {NULL, NULL};
            if (child->token.type == COMMAND) {
                for (int j = 0; j < child->numChildNodes; j++) {
                    ASTNode* child_of_child = child->childNodes[j];
                    printf("child of child: %s\n", child_of_child->token.value);
                    arguments[j] = child_of_child->token.value;
                }
            } else if (child->token.type == MESSAGE_TYPE) {
                arguments[0] = child->token.value;
                printf("child_message: %s\n", child->token.value);
            }
            processNode(child, arguments);
        }
    }
    freeAST(root);
}


// Function to free the AST when done transversing
void freeAST(ASTNode* root) {
    if (root == NULL) {
        return;
    }
    for (int i = 0; i < root->numChildNodes; i++) {
        freeAST(root->childNodes[i]);
    }
    free(root->childNodes);
    free(root);
}


// Functions of individual commands
void perform_complain() {
    printf("Complain: This is a generic complaint.\n");
}

void perform_print(char* arguments[]) {
    char* message = arguments[0];
    printf("Printing message: %s\n", message);
}

// REVISE THIS FUNCTION
void perform_set(char* arguments[]) {
    char* variable = arguments[0];
    char* value = arguments[1];
    printf("Set: %s = %s\n", variable, value);
}


void perform_add(char* arguments[]) {
    int var1 = atoi(arguments[0]);
    int var2 = atoi(arguments[1]);
    printf("Add: %d + %d = %d\n", var1, var2, var1+var2);
}


void perform_isequal(char* arguments[]) {
    int var1 = atoi(arguments[0]);
    int var2 = atoi(arguments[1]);
    if (var1 == var2) {
        printf("IsEqual: %d == %d\n", var1, var2);
    } else {
        printf("IsEqual: %d NOT EQUAL %d\n", var1, var2);
    }
}


int main() {
        FILE *fp;
        fp = fopen("test_file.txt", "r");
        if (fp == NULL) {
            printf("ERROR RUNNING FILE\n");
            return 1;
        }

        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *contents = (char *) malloc(file_size * sizeof(char));
        if (contents == NULL) {
            printf("Memory allocation error for contents\n");
            fclose(fp);
            return 1;
        }

        // Read file contents into the 'contents' buffer
        fread(contents, sizeof(char), file_size, fp);

        Token *token_list = (Token *) malloc(file_size * sizeof(Token));
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
        ASTNode *root = parseTokensIntoAST(token_list, num_tokens);

        // Print the AST tree
        printAST(root, 0);

        // interpreter(root);
        interpreter(root);

        // Free the memory
        free(lexer.word);
        free(contents);
        free(token_list);
        fclose(fp);

        return 0;
    }

    /* TO DO LIST
    * Make the set function better
    * Make the interpreter return errors if syntax incorrect
    * General error handling
     * If we want nested commands we have to chnage tree strcuture
     * Expand command list
     */