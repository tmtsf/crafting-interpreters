package com.github.tmtsf.lox.visitor;

import com.github.tmtsf.lox.ast.stmt.*;

public interface StmtVisitor<R> {
  R visit(Expression stmt);
  R visit(Print stmt);
  R visit(Var stmt);
  R visit(Block stmt);
  R visit(If stmt);
  R visit(While stmt);
  R visit(Function stmt);
  R visit(Return stmt);
}
