package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.visitor.ExprVisitor;

public abstract class Expr {
  public abstract <R> R accept(ExprVisitor<R> exprVisitor);
}
