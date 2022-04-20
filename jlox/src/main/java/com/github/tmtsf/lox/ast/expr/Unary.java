package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.visitor.ExprVisitor;
import com.github.tmtsf.lox.scanner.Token;

public class Unary extends Expr {
  private final Token operator;
  private final Expr right;

  public Unary(Token operator, Expr right) {
    this.operator = operator;
    this.right = right;
  }

  public Token getOperator() {
    return operator;
  }

  public Expr getRight() {
    return right;
  }

  @Override
  public <R> R accept(ExprVisitor<R> exprVisitor) {
    return exprVisitor.visit(this);
  }
}
