package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.visitor.StmtVisitor;

import java.util.List;

public class Block extends Stmt {
  private final List<Stmt> statements;

  public Block(List<Stmt> statements) {
    this.statements = statements;
  }

  public List<Stmt> getStatements() {
    return statements;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
