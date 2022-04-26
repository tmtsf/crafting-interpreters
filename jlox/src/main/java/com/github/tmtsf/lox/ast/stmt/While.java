package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.visitor.ExprVisitor;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class While extends Stmt {
  private final Expr condition;
  private final Stmt body;

  public While(Expr condition, Stmt body) {
    this.condition = condition;
    this.body = body;
  }

  public Expr getCondition() {
    return condition;
  }

  public Stmt getBody() {
    return body;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
