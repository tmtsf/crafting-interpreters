# JLox

## Scanner

Types of token that are supported by the Lox language are given in the following.

```Java
public enum TokenType {
  // Single-character tokens
  LEFT_PAREN,        // (
  RIGHT_PAREN,       // )
  LEFT_BRACE,        // {
  RIGHT_BRACE,       // }
  COMMA,             // ,
  DOT,               // .
  MINUS,             // -
  PLUS,              // +
  SEMICOLON,         // ;
  SLASH,             // /
  STAR,              // *

  // One or two character tokens
  BANG,              // !
  BANG_EQUAL,        // !=
  EQUAL,             // =
  EQUAL_EQUAL,       // ==
  GREATER,           // >
  GREATER_EQUAL,     // >=
  LESS,              // <
  LESS_EQUAL,        // <=

  // Literals
  IDENTIFIER,        // [a-zA-Z_]([a-zA-Z_0-9])*
  STRING,            // ""
  NUMBER,            // ([0-9])+(.[0-9]+)?

  // Keywords
  AND,               // and
  CLASS,             // class, class definition
  ELSE,              // else
  FALSE,             // false
  FUN,               // fun, function definition
  FOR,               // for
  IF,                // if
  NIL,               // nil, null object
  OR,                // or
  PRINT,             // print
  RETURN,            // return
  SUPER,             // super, class inheritance
  THIS,              // this
  TRUE,              // true
  VAR,               // var, variable declaration
  WHILE,             // while

  EOF                // end of file
}
```

A token is represented by its type, lexeme (a string representing the token), literal (the actual value of the token),
and the line number it appears in the source code.

```Java
public class Token {
  final TokenType type;
  final String lexeme;
  final Object literal;
  final int line;

  Token(TokenType type,
        String lexeme,
        Object literal,
        int line) {
    this.type = type;
    this.lexeme = lexeme;
    this.literal = literal;
    this.line = line;
  }
}
```

To scan the source code,
+ For single character tokens, consume the character, add the token, and proceed;
+ For one or two character tokens, check the following character:
  + If it cannot be used to form a two-character token with the previous character, add the single character token, and proceed;
  + Otherwise, consume the character, add the two-character token, and proceed;
+ For slash (/), if the following character is also slash, treat the remaining the line as comment. Otherwise, add the slash token;
+ For white spaces, ignore and proceed;
+ For new line, increment the line number and proceed;
+ For double quote ("), form a string with the content until the next double quote;
+ For numbers, form the corresponding floating number;
+ For characters, form the literal. If it is one of the keywords, add the corresponding token.

## Expressions

The Backus-Naur Form (BNF) for the expressions that we will parse is given below.

```
expression      -> equality ;
equality        -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison      -> term ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term            -> factor ( ( "+" | "-" ) factor )* ;
factor          -> unary ( ( "*" | "/" ) unary )* ;
unary           -> ( "-" | "!" ) unary
                 | primary ;
primary         -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
```

Here, we first introduce an abstract class to represent all the expressions in a tree form, known as the abstract
syntax tree (AST).

```Java
public abstract class Expr {
  public abstract <R> R accept(ExprVisitor<R> exprVisitor);
}
```

The `accept` method is an essential component of the visitor design pattern, which is heavily used in compilers for
processing the AST. For different types of expressions, we have concrete implementations of the abstract `Expr` class.
Parsing is performed in a recursive descent fashion.

The `Interpreter` is an implementation of `ExprVisitor<Object>`.

```Java
public class Interpreter implements ExprVisitor<Object> {
  // ...
}
```

It implements the `visit` method for all `Expr` types. Now, we have a simple interpreter
which can only parse and interpret a *single* line of expression.

## Statements

Adding statements, the BNF becomes

```
program         -> declaration* EOF ;
declaration     -> varDecl | statement ;
varDecl         -> "var" IDENTIFIER ( "=" expression )? ";" ;
statement       -> exprStmt | printStmt | block ;

exprStmt        -> expression ";" ;
printStmt       -> "print" expression ";" ;
block           -> "{" declaration* "}" ;

expression      -> assignment ;
assignment      -> IDENTIFIER "=" assignment | equality ;
equality        -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison      -> term ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term            -> factor ( ( "+" | "-" ) factor )* ;
factor          -> unary ( ( "*" | "/" ) unary )* ;
unary           -> ( "-" | "!" ) unary | primary ;
primary         -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
```

The `Interpreter` also needs to handle statements, so it implements
`StmtVisitor<Void>`:

```Java
public class Interpreter implements ExprVisitor<Object>, StmtVisitor<Void> {
  // ...
}
```

Since we have introduced variable declaration statement, we need an object to associate variables to values that they have
stored.

```Java
class Environment {
  private final Map<String, Object> values = new HashMap<>();
}
```

Then, we can define, retrieve, and update a variable value. By chaining the `Environment`'s together, we are also able to
manage scopes for variables.

## Adding control flow

Adding control flow, the BNF becomes

```
program         -> declaration* EOF ;
declaration     -> varDecl | statement ;
varDecl         -> "var" IDENTIFIER ( "=" expression )? ";" ;
statement       -> exprStmt | ifStmt | whileStmt printStmt | block ;

exprStmt        -> expression ";" ;
ifStmt          -> "if" "(" expression ")" statement ( "else" statement )? ;
whileStmt       -> "while" "(" expression ")" statement ;
printStmt       -> "print" expression ";" ;
block           -> "{" declaration* "}" ;

expression      -> assignment ;
assignment      -> IDENTIFIER "=" assignment | logic_or ;
logic_or        -> logic_and ( "or" logic_and )* ;
logic_and       -> equality ( "and" equality )* ;
equality        -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison      -> term ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term            -> factor ( ( "+" | "-" ) factor )* ;
factor          -> unary ( ( "*" | "/" ) unary )* ;
unary           -> ( "-" | "!" ) unary | primary ;
primary         -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
```

We will not directly support for loop with another `forStmt`. Rather, we will implement it as an equivalent `whileStmt`.

## Adding functions

With functions, the BNF is

```
program         -> declaration* EOF ;
declaration     -> funDecl | varDecl | statement ;
funDecl         -> "fun" function ;
varDecl         -> "var" IDENTIFIER ( "=" expression )? ";" ;
statement       -> exprStmt | ifStmt | whileStmt printStmt | block ;

function        -> IDENTIFIER "(" parameters? ")" block;
exprStmt        -> expression ";" ;
ifStmt          -> "if" "(" expression ")" statement ( "else" statement )? ;
whileStmt       -> "while" "(" expression ")" statement ;
printStmt       -> "print" expression ";" ;
returnStmt      -> "return" expression? ";" ;
block           -> "{" declaration* "}" ;

expression      -> assignment ;
assignment      -> IDENTIFIER "=" assignment | logic_or ;
logic_or        -> logic_and ( "or" logic_and )* ;
logic_and       -> equality ( "and" equality )* ;
equality        -> comparison ( ( "!=" | "==" ) comparison )* ;
comparison      -> term ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term            -> factor ( ( "+" | "-" ) factor )* ;
factor          -> unary ( ( "*" | "/" ) unary )* ;
unary           -> ( "-" | "!" ) unary | call ;
call            -> primary ( "(" arguments? ")" )* ;
primary         -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;

arguments       -> expression ( "," expression )* ;
parameters      -> IDENTIFIER ( "," IDENTIFIER )* ;
```



