package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.visitor.StmtVisitor;

public abstract class Stmt {
  public abstract <R> R accept(StmtVisitor<R> exprVisitor);
}
