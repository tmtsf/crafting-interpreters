package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.ast.expr.Expr;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.StmtVisitor;

public class Return extends Stmt {
  private final Token keyword;
  private final Expr value;

  public Return(Token keyword, Expr value) {
    this.keyword = keyword;
    this.value = value;
  }

  public Token getKeyword() {
    return keyword;
  }

  public Expr getValue() {
    return value;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
