package com.github.tmtsf.lox.ast.expr;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

import java.util.List;

public class Call extends Expr {
  private final Expr callee;
  private final Token token;
  private final List<Expr> arguments;

  public Call(Expr callee, Token token, List<Expr> arguments) {
    this.callee = callee;
    this.token = token;
    this.arguments = arguments;
  }

  public Expr getCallee() {
    return callee;
  }

  public Token getToken() {
    return token;
  }

  public List<Expr> getArguments() {
    return arguments;
  }

  @Override
  public <R> R accept(ExprVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
