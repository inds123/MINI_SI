#include <stdio.h>
#include <stdlib.h>

// Types of tokens
typedef enum 
{
    INTEGER, leFLOAT, STRING,
    PLUS, MINUS, MULT, DIV, ASSIGN,
    EQUAL, NOT_EQUAL, LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL,
    IF, ELSE, FOR, WHILE, READ, WRITE,
    SEMICOLON,
    
} TokenType;

// Token structure
typedef struct 
{
    TokenType type;
    char* literal;
} Token;

// Function that creates the keyword list
const char* keywords[] = 
{
    "if",
    "else",
    "for",
    "while",
    "read",
    "write",
    // Add other keywords as needed
};


// Function that checks whether the string is a keyword
int isMini_Si(const char* str) 
{
    
}




