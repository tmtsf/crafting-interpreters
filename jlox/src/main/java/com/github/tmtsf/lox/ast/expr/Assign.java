package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Assign extends Expr {
  private final Token name;
  private final Expr value;

  public Assign(Token name, Expr value) {
    this.name = name;
    this.value = value;
  }

  public Token getName() {
    return name;
  }

  public Expr getValue() {
    return value;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
