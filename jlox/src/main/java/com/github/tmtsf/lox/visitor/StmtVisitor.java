package com.github.tmtsf.lox.visitor;

import com.github.tmtsf.lox.ast.stmt.Block;
import com.github.tmtsf.lox.ast.stmt.Expression;
import com.github.tmtsf.lox.ast.stmt.Print;
import com.github.tmtsf.lox.ast.stmt.Var;

public interface StmtVisitor<R> {
  R visit(Expression stmt);
  R visit(Print stmt);
  R visit(Var stmt);
  R visit(Block stmt);
}
