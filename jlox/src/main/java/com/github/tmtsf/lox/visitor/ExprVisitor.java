package com.github.tmtsf.lox.visitor;

import com.github.tmtsf.lox.ast.expr.Binary;
import com.github.tmtsf.lox.ast.expr.Grouping;
import com.github.tmtsf.lox.ast.expr.Literal;
import com.github.tmtsf.lox.ast.expr.Unary;

public interface ExprVisitor<R> {
  R visit(Binary binary);
  R visit(Unary unary);
  R visit(Literal literal);
  R visit(Grouping grouping);
}
