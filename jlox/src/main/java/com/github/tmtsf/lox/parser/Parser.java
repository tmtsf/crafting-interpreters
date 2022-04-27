package com.github.tmtsf.lox.parser;

import com.github.tmtsf.lox.Lox;
import com.github.tmtsf.lox.ast.expr.*;
import com.github.tmtsf.lox.ast.stmt.*;
import com.github.tmtsf.lox.exception.ParseError;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.scanner.TokenType;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.logging.Level;

import static com.github.tmtsf.lox.scanner.TokenType.*;

public class Parser {

  private final List<Token> tokens;
  private int current = 0;

  public Parser(List<Token> tokens) {
    this.tokens = tokens;
  }

//  public Expr parse() {
//    try {
//      return expression();
//    } catch (ParseError e) {
//      return null;
//    }
//  }

  public List<Stmt> parse() {
    List<Stmt> statements = new ArrayList<>();
    while (!isAtEnd())
      statements.add(declaration());

    return statements;
  }

  private Stmt declaration() {
    try {
      if (match(FUN))
        return funDeclaration("function");

      if (match(VAR))
        return varDeclaration();

      return statement();
    } catch (ParseError e) {
      synchronize();
      return null;
    }
  }

  private Stmt funDeclaration(String kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    List<Token> parameters = new ArrayList<>();
    if (!check(RIGHT_PAREN)) {
      do {
        if (parameters.size() >= 255)
          error(peek(), "Cannot have more than 255 parameters.");

        parameters.add(consume(IDENTIFIER, "Expect parameter name."));
      } while (match(COMMA));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    List<Stmt> body = block();
    return new Function(name, parameters, body);
  }

  private Stmt varDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");

    Expr initializer = null;
    if (match(EQUAL))
      initializer = expression();

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return new Var(name, initializer);
  }

  private Stmt statement() {
    if (match(IF))
      return ifStatement();

    if (match(WHILE))
      return whileStatement();

    if (match(FOR))
      return forStatement();

    if (match(PRINT))
      return printStatement();

    if (match(RETURN))
      return returnStatement();

    if (match(LEFT_BRACE))
      return new Block(block());

    return expressionStatement();
  }

  private Stmt ifStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    Expr condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    Stmt thenBranch = statement();
    Stmt elseBranch = null;
    if (match(ELSE))
      elseBranch = statement();

    return new If(condition, thenBranch, elseBranch);
  }

  private Stmt whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    Expr condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    Stmt body = statement();

    return new While(condition, body);
  }

  // Transform the for loop:
  // for ( initializer ; condition ; increment )
  //   body ;
  //
  // to an equivalent while loop:
  // initializer ;
  // while ( condition ) {
  //   body ;
  //   increment ;
  // }
  private Stmt forStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    Stmt initializer;
    if (match(SEMICOLON))
      initializer = null;
    else if (match(VAR))
      initializer = varDeclaration();
    else
      initializer = expressionStatement();

    Expr condition = null;
    if (!check(SEMICOLON))
      condition = expression();

    consume(SEMICOLON, "Expect ';' after loop condition.");

    Expr increment = null;
    if (!check(RIGHT_PAREN))
      increment = expression();

    consume(RIGHT_PAREN, "Expect ')' after for clauses.");
    Stmt body = statement();

    if (increment != null)
      body = new Block(Arrays.asList(body, new Expression(increment)));

    if (condition == null)
      condition = new Literal(true);

    body = new While(condition, body);
    if (initializer != null)
      body = new Block(Arrays.asList(initializer, body));

    return body;
  }

  private List<Stmt> block() {
    List<Stmt> statements = new ArrayList<>();

    while (!check(RIGHT_BRACE) && !isAtEnd())
      statements.add(declaration());

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  private Stmt printStatement() {
    Expr value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return new Print(value);
  }

  private Stmt expressionStatement() {
    Expr expr = expression();
    consume(SEMICOLON, "Expect ';' after semicolon.");
    return new Expression(expr);
  }

  private Stmt returnStatement() {
    Token keyword = previous();
    Expr value = null;
    if (!check(SEMICOLON))
      value = expression();

    consume(SEMICOLON, "Expect ';' after return value.");
    return new Return(keyword, value);
  }

  private Expr expression() {
    return assignment();
  }

  private Expr assignment() {
    Expr expr = or();

    if (match(EQUAL)) {
      Token equals = previous();
      Expr value = assignment();

      if (expr instanceof Variable) {
        Token name = ((Variable) expr).getName();
        return new Assign(name, value);
      }

      error(equals, "Invalid assignment target.");
    }

    return expr;
  }

  private Expr or() {
    Expr expr = and();

    while (match(OR)) {
      Token operator = previous();
      Expr right = and();
      expr = new Logical(expr, operator, right);
    }

    return expr;
  }

  private Expr and() {
    Expr expr = equality();

    while (match(AND)) {
      Token operator = previous();
      Expr right = equality();
      expr = new Logical(expr, operator, right);
    }

    return expr;
  }

  private Expr equality() {
    Expr expr = comparison();

    while (match(BANG_EQUAL, BANG)) {
      Token operator = previous();
      Expr right = comparison();
      expr = new Binary(expr, operator, right);
    }

    return expr;
  }

  private Expr comparison() {
    Expr expr = term();

    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
      Token operator = previous();
      Expr right = term();
      expr = new Binary(expr, operator, right);
    }

    return expr;
  }

  private Expr term() {
    Expr expr = factor();

    while (match(MINUS, PLUS)) {
      Token operator = previous();
      Expr right = factor();
      expr = new Binary(expr, operator, right);
    }

    return expr;
  }

  private Expr factor() {
    Expr expr = unary();

    while (match(SLASH, STAR)) {
      Token operator = previous();
      Expr right = unary();
      expr = new Binary(expr, operator, right);
    }

    return expr;
  }

  private Expr unary() {
    if (match(BANG, MINUS)) {
      Token operator = previous();
      Expr right = unary();
      return new Unary(operator, right);
    }

    return call();
  }

  private Expr call() {
    Expr expr = primary();

    while (true) {
      if (match(LEFT_PAREN))
        expr = finishCall(expr);
      else
        break;
    }

    return expr;
  }

  private Expr primary() {
    if (match(FALSE))
      return new Literal(false);
    if (match(TRUE))
      return new Literal(true);
    if (match(NIL))
      return new Literal(null);

    if (match(NUMBER, STRING))
      return new Literal(previous().getLiteral());

    if (match(IDENTIFIER))
      return new Variable(previous());

    if (match(LEFT_PAREN)) {
      Expr expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return new Grouping(expr);
    }

    throw error(peek(), "Expect expression.");
  }

  // Utility functions
  private boolean match(TokenType... types) {
    for (var type : types) {
      if (check(type)) {
        advance();
        return true;
      }
    }

    return false;
  }

  private boolean check(TokenType type) {
    if (isAtEnd())
      return false;

    return peek().getType() == type;
  }

  private Token advance() {
    if (!isAtEnd())
      ++current;

    return previous();
  }

  private boolean isAtEnd() {
    return peek().getType() == EOF;
  }

  private Token peek() {
    return tokens.get(current);
  }

  private Token previous() {
    return tokens.get(current - 1);
  }

  private Expr finishCall(Expr callee) {
    List<Expr> arguments = new ArrayList<>();
    if (!check(RIGHT_PAREN)) {
      do {
        if (arguments.size() >= 255)
          error(peek(), "Cannot have more than 255 arguments.");

        arguments.add(expression());
      } while (match(COMMA));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return new Call(callee, paren, arguments);
  }

  // Error handling
  private Token consume(TokenType type, String message) {
    if (check(type))
      return advance();

    throw error(peek(), message);
  }

  private ParseError error(Token token, String message) {
    Lox.error(token, message);
    return new ParseError();
  }

  private void synchronize() {
    advance();

    while (!isAtEnd()) {
      if (previous().getType() == SEMICOLON)
        return;

      switch (peek().getType()) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
        default:
          // Not the beginning of a new statement? Keep on going!
          break;
      }

      advance();
    }
  }
}
