package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Grouping extends Expr {
  private final Expr expr;

  public Grouping(Expr expr) {
    this.expr = expr;
  }

  public Expr getExpr() {
    return expr;
  }

  @Override
  public <R> R accept(ExprVisitor<R> exprVisitor) {
    return exprVisitor.visit(this);
  }
}
