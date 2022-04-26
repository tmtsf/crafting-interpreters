package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Logical extends Expr {
  private final Expr left;
  private final Token operator;
  private final Expr right;

  public Logical(Expr left, Token operator, Expr right) {
    this.left = left;
    this.operator = operator;
    this.right = right;
  }

  public Expr getLeft() {
    return left;
  }

  public Token getOperator() {
    return operator;
  }

  public Expr getRight() {
    return right;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
