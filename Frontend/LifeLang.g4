grammar LifeLang;

program: funcDecl* EOF;

varDeclList: varDeclItem (COMMA varDeclItem)*;

varDeclItem:
    ID (ASSIGN expr)?
    | ID LBRACK INT_LITERAL COMMA INT_LITERAL RBRACK (ASSIGN arrayInitializer)?;

arrayInitializer: LBRACE expr (COMMA expr)* RBRACE;

funcDecl: FUNC ID LPAREN paramList? RPAREN block;

paramList: paramItem (COMMA paramItem)*;

paramItem:
        ID
        | ID LBRACK INT_LITERAL COMMA INT_LITERAL RBRACK;

block: LBRACE stmt* RBRACE;

stmt:
    VAR varDeclList SEMI
    | assignStmt SEMI
    | ifStmt
    | loopStmt
    | expr SEMI
    | RETURN expr? SEMI;

assignStmt:
    ID ASSIGN expr
    | ID LBRACK expr COMMA expr RBRACK ASSIGN expr;

ifStmt: IF LPAREN expr RPAREN block (ELSE block)?;

loopStmt: LOOP block | LOOP LPAREN expr SEMI expr RPAREN block;

expr: logicalOrExpr;

logicalOrExpr: logicalAndExpr (OR logicalAndExpr)*;

logicalAndExpr: equalityExpr (AND equalityExpr)*;

equalityExpr: relationalExpr ((EQ | NEQ) relationalExpr)*;

relationalExpr: additiveExpr ((LT | LE | GT | GE) additiveExpr)*;

additiveExpr: multiplicativeExpr ((PLUS | MINUS) multiplicativeExpr)*;

multiplicativeExpr: unaryExpr ((STAR | SLASH | PERCENT) unaryExpr)*;

unaryExpr:
    MINUS unaryExpr
    | NOT unaryExpr
    | ID INC_OP
    | primaryExpr;

primaryExpr:
    INT_LITERAL
    | ID
    | ID LPAREN argList? RPAREN
    | ID LBRACK expr COMMA expr RBRACK
    | LPAREN expr RPAREN;

argList: expr (COMMA expr)*;

// Lexer rules

VAR: 'var';
FUNC: 'func';
IF: 'if';
ELSE: 'else';
LOOP: 'loop';
RETURN: 'return';

INC_OP: '++';
PLUS: '+';
MINUS: '-';
STAR: '*';
SLASH: '/';
PERCENT: '%';

LT: '<';
LE: '<=';
GT: '>';
GE: '>=';
EQ: '==';
NEQ: '!=';
AND: '&&';
OR: '||';
NOT: '!';

ASSIGN: '=';

LBRACE: '{';
RBRACE: '}';
LPAREN: '(';
RPAREN: ')';
LBRACK: '[';
RBRACK: ']';
COMMA: ',';
SEMI: ';';

INT_LITERAL: [0-9]+;

ID: [a-zA-Z_][a-zA-Z_0-9]*;

WS: [ \t\r\n]+ -> skip;