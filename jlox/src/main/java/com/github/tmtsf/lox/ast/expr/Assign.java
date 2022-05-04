package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Assign extends Expr {
  private final Token token;
  private final Expr value;

  public Assign(Token token, Expr value) {
    this.token = token;
    this.value = value;
  }

  public Token getToken() {
    return token;
  }

  public Expr getValue() {
    return value;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
