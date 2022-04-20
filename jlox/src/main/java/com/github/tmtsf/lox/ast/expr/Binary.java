package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.visitor.ExprVisitor;
import com.github.tmtsf.lox.scanner.Token;

public class Binary extends Expr {
  private final Expr left;
  private final Token operator;
  private final Expr right;

  public Binary(Expr left, Token operator, Expr right) {
    this.left = left;
    this.operator = operator;
    this.right = right;
  }

  public Expr getLeft() {
    return left;
  }

  public Expr getRight() {
    return right;
  }

  public Token getOperator() {
    return operator;
  }

  @Override
  public <R> R accept(ExprVisitor<R> exprVisitor) {
    return exprVisitor.visit(this);
  }
}
