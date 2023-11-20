#ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    INTEGER, FLOAT, STRING,
    PLUS, MINUS, MULT, DIV, ASSIGN,
    EQUAL, NOT_EQUAL, LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL,
    IF, ELSE, FOR, WHILE, READ, WRITE,
    SEMICOLON,
} TokenType;


// Token structure
typedef struct {
    TokenType type;
    char* literal;
} Token;

#endif  // TOKEN_H