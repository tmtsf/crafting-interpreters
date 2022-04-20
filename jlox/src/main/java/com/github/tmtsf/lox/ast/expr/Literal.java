package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Literal extends Expr {
  private final Object value;

  public Literal(Object value) {
    this.value = value;
  }

  public Object getValue() {
    return value;
  }

  @Override
  public <R> R accept(ExprVisitor<R> exprVisitor) {
    return exprVisitor.visit(this);
  }
}
