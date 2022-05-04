package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class Var extends Stmt {
  private final Token token;
  private final Expr initializer;

  public Var(Token token, Expr initializer) {
    this.token = token;
    this.initializer = initializer;
  }

  public Token getToken() {
    return token;
  }

  public Expr getInitializer() {
    return initializer;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
