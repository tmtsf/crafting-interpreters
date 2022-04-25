package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class Print extends Stmt {
  private final Expr expression;

  public Print(Expr expression) {
    this.expression = expression;
  }

  public Expr getExpression() {
    return expression;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
