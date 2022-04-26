package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class If extends Stmt {
  private final Expr condition;
  private final Stmt thenBranch;
  private final Stmt elseBranch;

  public If(Expr condition, Stmt thenBranch, Stmt elseBranch) {
    this.condition = condition;
    this.thenBranch = thenBranch;
    this.elseBranch = elseBranch;
  }

  public Expr getCondition() {
    return condition;
  }

  public Stmt getThenBranch() {
    return thenBranch;
  }

  public Stmt getElseBranch() {
    return elseBranch;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
