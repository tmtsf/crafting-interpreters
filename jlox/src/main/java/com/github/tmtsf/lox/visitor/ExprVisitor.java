package com.github.tmtsf.lox.visitor;

import com.github.tmtsf.lox.ast.expr.*;

public interface ExprVisitor<R> {
  R visit(Binary expr);
  R visit(Unary expr);
  R visit(Literal expr);
  R visit(Grouping expr);
  R visit(Variable expr);
  R visit(Assign expr);
  R visit(Logical expr);
}
