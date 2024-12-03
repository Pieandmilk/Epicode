#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Define token types
typedef enum {
    TOKEN_VAR, TOKEN_ACTION, TOKEN_MAIN, TOKEN_IF,TOKEN_ELIF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR,
    TOKEN_PRINT, TOKEN_INPUT, TOKEN_RETURN, TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_STRING_LITERAL, TOKEN_OPERATOR, TOKEN_COMPARATOR,TOKEN_LOGICAL_OPERATOR, TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_ASSIGN, TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char lexeme[100];
} Token;

// Global variables
char *sourceCode;
int currentPos = 0;

// Function prototypes
Token getNextToken();
void printToken(Token token);
void error(const char *message);

// Utility functions
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int isComparator(char c) {
    return c == '=' || c == '!' || c == '<' || c == '>';
}

// Lexer: Get the next token
Token getNextToken() {
    Token token;
    token.lexeme[0] = '\0';

    // Skip whitespace
    while (isspace(sourceCode[currentPos])) {
        currentPos++;
    }

    // End of file
    if (sourceCode[currentPos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    // Logical operators
    if (sourceCode[currentPos] == '&' && sourceCode[currentPos + 1] == '&') {
        token.type = TOKEN_LOGICAL_OPERATOR;
        strcpy(token.lexeme, "&&");
        currentPos += 2;
        return token;
    } else if (sourceCode[currentPos] == '|' && sourceCode[currentPos + 1] == '|') {
        token.type = TOKEN_LOGICAL_OPERATOR;
        strcpy(token.lexeme, "||");
        currentPos += 2;
        return token;
    } else if (sourceCode[currentPos] == '!') {
        token.type = TOKEN_LOGICAL_OPERATOR;
        strcpy(token.lexeme, "!");
        currentPos++;
        return token;
    }

    // Keywords and identifiers
    if (isalpha(sourceCode[currentPos]) || sourceCode[currentPos] == '_') {
        int start = currentPos;
        while (isalnum(sourceCode[currentPos]) || sourceCode[currentPos] == '_') {
            currentPos++;
        }
        strncpy(token.lexeme, sourceCode + start, currentPos - start);
        token.lexeme[currentPos - start] = '\0';

        // Check for keywords
        if (strcmp(token.lexeme, "var") == 0) token.type = TOKEN_VAR;
        else if (strcmp(token.lexeme, "action") == 0) token.type = TOKEN_ACTION;
        else if (strcmp(token.lexeme, "main") == 0) token.type = TOKEN_MAIN;
        else if (strcmp(token.lexeme, "if") == 0) token.type = TOKEN_IF;
        else if (strcmp(token.lexeme, "elif") == 0) token.type = TOKEN_ELIF;
        else if (strcmp(token.lexeme, "else") == 0) token.type = TOKEN_ELSE;
        else if (strcmp(token.lexeme, "while") == 0) token.type = TOKEN_WHILE;
        else if (strcmp(token.lexeme, "for") == 0) token.type = TOKEN_FOR;
        else if (strcmp(token.lexeme, "print") == 0) token.type = TOKEN_PRINT;
        else if (strcmp(token.lexeme, "input") == 0) token.type = TOKEN_INPUT;
        else if (strcmp(token.lexeme, "return") == 0) token.type = TOKEN_RETURN;
        else token.type = TOKEN_IDENTIFIER;

        return token;
    }

    // Numbers
    if (isdigit(sourceCode[currentPos])) {
        int start = currentPos;
        while (isdigit(sourceCode[currentPos])) {
            currentPos++;
        }
        strncpy(token.lexeme, sourceCode + start, currentPos - start);
        token.lexeme[currentPos - start] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // String literals
    if (sourceCode[currentPos] == '"') {
        int start = ++currentPos;
        while (sourceCode[currentPos] != '"' && sourceCode[currentPos] != '\0') {
            currentPos++;
        }
        if (sourceCode[currentPos] == '\0') {
            error("Unterminated string literal");
        }
        strncpy(token.lexeme, sourceCode + start, currentPos - start);
        token.lexeme[currentPos - start] = '\0';
        token.type = TOKEN_STRING_LITERAL;
        currentPos++;
        return token;
    }

    // Operators and punctuation
    char c = sourceCode[currentPos++];
    switch (c) {
        case '+': case '-': case '*': case '/':
            token.type = TOKEN_OPERATOR;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            break;
        case '=':
		    if (sourceCode[currentPos] == '=') {  // Check for equality operator (==)
		        token.type = TOKEN_COMPARATOR;
		        token.lexeme[0] = c;
		        token.lexeme[1] = '=';
		        token.lexeme[2] = '\0';
		        currentPos++;
		    } else {  // Assignment operator (=)
		        token.type = TOKEN_ASSIGN;
		        token.lexeme[0] = c;
		        token.lexeme[1] = '\0';
		    }
		    break;
		case '!': case '<': case '>':
		    token.type = TOKEN_COMPARATOR;
		    token.lexeme[0] = c;
		    token.lexeme[1] = '\0';
		    if (sourceCode[currentPos] == '=') {
		        token.lexeme[1] = '=';
		        token.lexeme[2] = '\0';
		        currentPos++;
		    }
		    break;
        case '(': token.type = TOKEN_LPAREN; strcpy(token.lexeme, "("); break;
        case ')': token.type = TOKEN_RPAREN; strcpy(token.lexeme, ")"); break;
        case '{': token.type = TOKEN_LBRACE; strcpy(token.lexeme, "{"); break;
        case '}': token.type = TOKEN_RBRACE; strcpy(token.lexeme, "}"); break;
        case ';': token.type = TOKEN_SEMICOLON; strcpy(token.lexeme, ";"); break;
        case ',': token.type = TOKEN_COMMA; strcpy(token.lexeme, ","); break;
        default:
            error("Unexpected character");
    }
    return token;
}

// Print token for debugging
void printToken(Token token) {
    printf("Token: %-15s Lexeme: %s\n", 
        (token.type == TOKEN_VAR) ? "TOKEN_VAR" :
        (token.type == TOKEN_ACTION) ? "TOKEN_ACTION" :
        (token.type == TOKEN_MAIN) ? "TOKEN_MAIN" :
        (token.type == TOKEN_IF) ? "TOKEN_IF" :
        (token.type == TOKEN_ELIF) ? "TOKEN_ELIF" :
        (token.type == TOKEN_ELSE) ? "TOKEN_ELSE" :
        (token.type == TOKEN_IDENTIFIER) ? "TOKEN_IDENTIFIER" :
        (token.type == TOKEN_NUMBER) ? "TOKEN_NUMBER" :
        (token.type == TOKEN_OPERATOR) ? "TOKEN_OPERATOR" :
        (token.type == TOKEN_LOGICAL_OPERATOR) ? "TOKEN_LOGICAL_OPERATOR" :
        (token.type == TOKEN_EOF) ? "TOKEN_EOF" : "UNKNOWN",
        token.lexeme);
}

// Current token
Token currentToken;

// Function prototypes for recursive-descent parsing
void parseGameProgram();
void parseMain();
void parseStatements();
void parseStatement();
void parsePrint();
void parseVarDecl();
void parseAssign();
void parseActionDecl();
void parseExpression();
void parseReturn();
void parseControlFlow();
void parseWhile();
void parseCondition();
void match(TokenType expectedType);
void error(const char *message);

// Get the next token from the lexer
void advance() {
    currentToken = getNextToken();
}

// Match the current token and advance
void match(TokenType expectedType) {
    if (currentToken.type == expectedType) {
        advance();
    } else {
        char errorMessage[100];
        sprintf(errorMessage, "Expected token of type %d, but got '%s'", expectedType, currentToken.lexeme);
        error(errorMessage);
    }
}

// Error handling for parsing
void error(const char *message) {
    fprintf(stderr, "Syntax Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Parse <GameProgram>
void parseGameProgram() {
    while (currentToken.type == TOKEN_ACTION || currentToken.type == TOKEN_MAIN) {
        if (currentToken.type == TOKEN_ACTION) {
            parseActionDecl();
        } else if (currentToken.type == TOKEN_MAIN) {
            parseMain();
        }
    }

    if (currentToken.type != TOKEN_EOF) {
        error("Unexpected tokens at the end of the program");
    }
}

// Parse <Main>
void parseMain() {
    match(TOKEN_MAIN);
    match(TOKEN_LBRACE);
    parseStatements();
    match(TOKEN_RBRACE);
}

// Parse <Statements>
void parseStatements() {
    while (currentToken.type == TOKEN_VAR || currentToken.type == TOKEN_PRINT || 
           currentToken.type == TOKEN_IDENTIFIER || currentToken.type == TOKEN_RETURN ||
           currentToken.type == TOKEN_IF || currentToken.type == TOKEN_WHILE || 
           currentToken.type == TOKEN_FOR || currentToken.type == TOKEN_ACTION) { // Added TOKEN_ACTION here
        parseStatement();
    }
}


// Parse <Statement>
void parseStatement() {
    if (currentToken.type == TOKEN_VAR) {
        parseVarDecl();
    } else if (currentToken.type == TOKEN_ACTION) {
        parseActionDecl();  // Allow action declarations within blocks
    } else if (currentToken.type == TOKEN_PRINT) {
		parsePrint();
    } else if (currentToken.type == TOKEN_IDENTIFIER) {
        // This can be an assignment or a function call
        advance();  // Consume the identifier
        if (currentToken.type == TOKEN_ASSIGN) {
			parseAssign();
        } else if (currentToken.type == TOKEN_LPAREN) {
            // Function call
            match(TOKEN_LPAREN);
            // Handle arguments here (if necessary)
            match(TOKEN_RPAREN);
            match(TOKEN_SEMICOLON);
        } else {
            error("Unexpected token after identifier");
        }
    } else if (currentToken.type == TOKEN_RETURN) {
		parseReturn();
	} else if (currentToken.type == TOKEN_IF) {
        parseControlFlow();
    } else if (currentToken.type == TOKEN_WHILE) {
        parseWhile();
    } else {
        error("Unexpected token in statement");
    }
}

//Parse <Print>
void parsePrint(){
    match(TOKEN_PRINT);
    match(TOKEN_LPAREN);
    parseExpression();
    match(TOKEN_RPAREN);
    match(TOKEN_SEMICOLON);
}


// Parse <VarDecl>
void parseVarDecl() {
    match(TOKEN_VAR);
    match(TOKEN_IDENTIFIER);
    match(TOKEN_ASSIGN);
    parseExpression();
    match(TOKEN_SEMICOLON);
}

// Parse <Assign>
void parseAssign(){
	match(TOKEN_ASSIGN);
	parseExpression();
    match(TOKEN_SEMICOLON);
}

// Parse <ActionDecl>
void parseActionDecl() {
    match(TOKEN_ACTION);
    match(TOKEN_IDENTIFIER);
    match(TOKEN_LPAREN);
    // Handle parameters here (if necessary)
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    parseStatements();
    match(TOKEN_RBRACE);
}

// Parse <Expression>
void parseExpression() {
    // Handle the first operand
    if (currentToken.type == TOKEN_NUMBER || currentToken.type == TOKEN_STRING_LITERAL || 
        currentToken.type == TOKEN_IDENTIFIER) {
        advance();  // Consume the token
    } else if (currentToken.type == TOKEN_LPAREN) { // Handle sub-expression
        match(TOKEN_LPAREN);
        parseExpression();
        match(TOKEN_RPAREN);
    } else {
        error("Invalid expression");
    }

    // Handle operators and subsequent operands
    while (currentToken.type == TOKEN_OPERATOR || currentToken.type == TOKEN_COMPARATOR || 
           currentToken.type == TOKEN_LOGICAL_OPERATOR) {
        advance();  // Consume the operator
        parseExpression();  // Parse the next operand or sub-expression
    }
}

//Parse <Return>
void parseReturn(){
    match(TOKEN_RETURN);
    parseExpression();
    match(TOKEN_SEMICOLON);
}

//Parse <ControlFlow>
void parseControlFlow() {
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    parseCondition();  // Parse the condition
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    parseStatements();  // Parse the body of the if
    match(TOKEN_RBRACE);

    // Check for optional elif or else blocks
    while (currentToken.type == TOKEN_ELIF) {
        match(TOKEN_ELIF);
        match(TOKEN_LPAREN);
        parseCondition();  // Parse the condition
        match(TOKEN_RPAREN);
        match(TOKEN_LBRACE);
        parseStatements();  // Parse the body of the elif
        match(TOKEN_RBRACE);
    }

    if (currentToken.type == TOKEN_ELSE) {
        match(TOKEN_ELSE);
        match(TOKEN_LBRACE);
        parseStatements();  // Parse the body of the else
        match(TOKEN_RBRACE);
    }
}

// Parse <WhileStatement>
void parseWhile() {
    match(TOKEN_WHILE);
    match(TOKEN_LPAREN);
    parseCondition();  // Parse the condition
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    parseStatements();  // Parse the body of the loop
    match(TOKEN_RBRACE);
}

// Parse <Condition>
void parseCondition() {
    // Handle negative numbers
    if (currentToken.type == TOKEN_OPERATOR && strcmp(currentToken.lexeme, "-") == 0) {
        advance();  // Consume the '-'
        if (currentToken.type == TOKEN_NUMBER) {
            advance();  // Consume the number
        } else {
            error("Invalid condition: Expected a number after '-'.");
        }
    } 
    // First operand (number, string, identifier, or sub-expression)
    else if (currentToken.type == TOKEN_NUMBER || 
             currentToken.type == TOKEN_STRING_LITERAL || 
             currentToken.type == TOKEN_IDENTIFIER) {
        advance();  // Consume the operand
    } else if (currentToken.type == TOKEN_LPAREN) { // Handle parentheses for sub-conditions
        match(TOKEN_LPAREN);
        parseCondition();
        match(TOKEN_RPAREN);
    } else if (currentToken.type == TOKEN_LOGICAL_OPERATOR && strcmp(currentToken.lexeme, "!") == 0) {
        advance();  // Consume the '!'
        if (currentToken.type == TOKEN_LPAREN) {
            match(TOKEN_LPAREN);
            parseCondition();
            match(TOKEN_RPAREN);
        } else {
            parseCondition();  // Parse the operand directly
        }
    } else {
        error("Invalid condition: Expected operand or sub-condition.");
    }

    // Optional comparator or logical operator
    while (currentToken.type == TOKEN_COMPARATOR || 
           (currentToken.type == TOKEN_LOGICAL_OPERATOR && 
            (strcmp(currentToken.lexeme, "&&") == 0 || strcmp(currentToken.lexeme, "||") == 0))) {
        advance();  // Consume the operator
        parseCondition();  // Parse the next condition
    }
}



// Main function to test the parser
int main() {
	sourceCode = 
	    "main { \n"
	    "var a = 5;"
	    "var b = 2;"
	
	    "var anwser = (2 * (3 + a) - (2 / b)) - 2;"
	    "print(\"Answer: \" + anwser);"
	    "}";
	    
    advance();  // Initialize the first token
    parseGameProgram();
    printf("Parsing completed successfully.\n");
    return 0;
}


