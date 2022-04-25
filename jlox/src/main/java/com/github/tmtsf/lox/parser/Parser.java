package com.github.tmtsf.lox.parser;

import com.github.tmtsf.lox.Lox;
import com.github.tmtsf.lox.ast.expr.*;
import com.github.tmtsf.lox.ast.stmt.*;
import com.github.tmtsf.lox.exception.ParseError;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.scanner.TokenType;

import java.util.ArrayList;
import java.util.List;

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
      if (match(VAR))
        return varDeclaration();

      return statement();
    } catch (ParseError e) {
      synchronize();
      return null;
    }
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
    if (match(PRINT))
      return printStatement();

    if (match(LEFT_BRACE))
      return new Block(block());

    return expressionStatement();
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

  private Expr expression() {
    return assignment();
  }

  private Expr assignment() {
    Expr expr = equality();

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

    return primary();
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

  }
}
