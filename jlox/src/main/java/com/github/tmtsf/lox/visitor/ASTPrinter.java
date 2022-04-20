package com.github.tmtsf.lox.visitor;

import com.github.tmtsf.lox.ast.expr.*;

public class ASTPrinter implements ExprVisitor<String> {
  public String print(Expr expr) {
    return expr.accept(this);
  }

  @Override
  public String visit(Binary expr) {
    return parenthesize(expr.getOperator().getLexeme(),
                        expr.getLeft(),
                        expr.getRight());
  }

  @Override
  public String visit(Unary expr) {
    return parenthesize(expr.getOperator().getLexeme(),
                        expr.getRight());
  }

  @Override
  public String visit(Literal expr) {
    if (expr.getValue() == null)
      return "nil";

    return expr.getValue().toString();
  }

  @Override
  public String visit(Grouping expr) {
    return parenthesize("group", expr.getExpr());
  }

  // Utility function
  private String parenthesize(String name, Expr... exprs) {
    StringBuilder builder = new StringBuilder();

    builder.append("(").append(name);
    for (var expr : exprs) {
      builder.append(" ");
      builder.append(expr.accept(this));
    }
    builder.append(")");

    return builder.toString();
  }
}
