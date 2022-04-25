package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Variable extends Expr {
  private final Token name;

  public Variable(Token name) {
    this.name = name;
  }

  public Token getName() {
    return name;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
