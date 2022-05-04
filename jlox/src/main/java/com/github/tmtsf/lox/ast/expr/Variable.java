package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Variable extends Expr {
  private final Token token;

  public Variable(Token token) {
    this.token = token;
  }

  public Token getToken() {
    return token;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
