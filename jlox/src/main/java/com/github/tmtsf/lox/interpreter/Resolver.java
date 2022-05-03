package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.Lox;
import com.github.tmtsf.lox.ast.expr.*;
import com.github.tmtsf.lox.ast.stmt.*;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;
import com.github.tmtsf.lox.visitor.StmtVisitor;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

public class Resolver implements ExprVisitor<Void>, StmtVisitor<Void> {
  private final Interpreter interpreter;
  private final Stack<Map<String, Boolean>> scopes = new Stack<>();

  public Resolver(Interpreter interpreter) {
    this.interpreter = interpreter;
  }

  @Override
  public Void visit(Block stmt) {
    beginScope();
    resolve(stmt.getStatements());
    endScope();
    return null;
  }

  @Override
  public Void visit(Var stmt) {
    declare(stmt.getName());
    if (stmt.getInitializer() != null)
      resolve(stmt.getInitializer());

    define(stmt.getName());
    return null;
  }

  @Override
  public Void visit(Variable expr) {
    if (!scopes.isEmpty() && scopes.peek().get(expr.getName().getLexeme()) == Boolean.FALSE)
      Lox.error(expr.getName(), "Cannot read local variable in its own initializer.");

    resolveLocal(expr, expr.getName());
    return null;
  }

  @Override
  public Void visit(Assign expr) {
    resolve(expr.getValue());
    resolveLocal(expr, expr.getName());
    return null;
  }

  @Override
  public Void visit(Function stmt) {
    declare(stmt.getName());
    define(stmt.getName());

    resolveFunction(stmt);
    return null;
  }

  @Override
  public Void visit(Expression stmt) {
    resolve(stmt.getExpression());
    return null;
  }

  @Override
  public Void visit(If stmt) {
    resolve(stmt.getCondition());
    resolve(stmt.getThenBranch());
    if (stmt.getElseBranch() != null)
      resolve(stmt.getElseBranch());

    return null;
  }

  @Override
  public Void visit(Print stmt) {
    resolve(stmt.getExpression());
    return null;
  }

  @Override
  public Void visit(Return stmt) {
    if (stmt.getValue() != null)
      resolve(stmt.getValue());

    return null;
  }

  @Override
  public Void visit(While stmt) {
    resolve(stmt.getCondition());
    resolve(stmt.getBody());
    return null;
  }

  @Override
  public Void visit(Binary expr) {
    resolve(expr.getLeft());
    resolve(expr.getRight());
    return null;
  }

  @Override
  public Void visit(Call expr) {
    resolve(expr.getCallee());

    for (var arg : expr.getArguments())
      resolve(arg);

    return null;
  }

  @Override
  public Void visit(Grouping expr) {
    resolve(expr.getExpr());
    return null;
  }

  @Override
  public Void visit(Literal expr) {
    return null;
  }

  @Override
  public Void visit(Logical expr) {
    resolve(expr.getLeft());
    resolve(expr.getRight());
    return null;
  }

  @Override
  public Void visit(Unary expr) {
    resolve(expr.getRight());
    return null;
  }


  // utility functions
  public void resolve(List<Stmt> statements) {
    for (var statement : statements)
      resolve(statement);
  }

  private void resolve(Stmt stmt) {
    stmt.accept(this);
  }

  private void resolve(Expr expr) {
    expr.accept(this);
  }

  private void resolveLocal(Expr expr, Token token) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
      if (scopes.get(i).containsKey(token.getLexeme())) {
        interpreter.resolve(expr, scopes.size() - 1 - i);
        return;
      }
    }
  }

  private void resolveFunction(Function function) {
    beginScope();
    for (var param : function.getParameters()) {
      declare(param);
      define(param);
    }

    resolve(function.getBody());
    endScope();
  }

  private void beginScope() {
    scopes.push(new HashMap<String, Boolean>());
  }

  private void endScope() {
    scopes.pop();
  }

  private void declare(Token token) {
    if (scopes.isEmpty())
      return;

    scopes.peek().put(token.getLexeme(), false);
  }

  private void define(Token token) {
    if (scopes.isEmpty())
      return;

    scopes.peek().put(token.getLexeme(), true);
  }
}
