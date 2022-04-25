package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class Var extends Stmt {
  private final Token name;
  private final Expr initializer;

  public Var(Token name, Expr initializer) {
    this.name = name;
    this.initializer = initializer;
  }

  public Token getName() {
    return name;
  }

  public Expr getInitializer() {
    return initializer;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
