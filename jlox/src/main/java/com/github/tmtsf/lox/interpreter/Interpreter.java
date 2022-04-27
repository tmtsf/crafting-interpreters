package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.Lox;
import com.github.tmtsf.lox.ast.expr.*;
import com.github.tmtsf.lox.ast.stmt.*;
import com.github.tmtsf.lox.exception.ReturnValueException;
import com.github.tmtsf.lox.exception.RuntimeError;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.scanner.TokenType;
import com.github.tmtsf.lox.visitor.ExprVisitor;
import com.github.tmtsf.lox.visitor.StmtVisitor;

import java.util.ArrayList;
import java.util.List;

public class Interpreter implements ExprVisitor<Object>, StmtVisitor<Void> {
  final Environment globals = new Environment();
  private Environment environment = globals;

  public Interpreter() {
    globals.define("clock", new LoxCallable() {
      @Override
      public int arity() {
        return 0;
      }

      @Override
      public Object call(Interpreter interpreter, List<Object> arguments) {
        return (double)System.currentTimeMillis() / 1000.0;
      }

      @Override
      public String toString() {
        return "<native fn>";
      }
    });
  }

  public void interpret(List<Stmt> statements) {
    try {
      for (var statement : statements)
        execute(statement);
    } catch (RuntimeError e) {
      Lox.runtimeError(e);
    }
  }

  @Override
  public Object visit(Binary expr) {
    Object left = evaluate(expr.getLeft());
    Object right = evaluate(expr.getRight());

    Token operator = expr.getOperator();
    switch (operator.getType()) {
      case MINUS:
        checkNumberOperands(operator, left, right);
        return (double)left - (double)right;
      case SLASH:
        checkNumberOperands(operator, left, right);
        return (double)left / (double)right;
      case STAR:
        checkNumberOperands(operator, left, right);
        return (double)left * (double)right;
      case PLUS:
        if (left instanceof Double && right instanceof Double) {
          return (double)left + (double)right;
        }

        if (left instanceof String && right instanceof String) {
          return (String)left + (String) right;
        }

        throw new RuntimeError(operator, "Can only perform addition for two numbers or two strings.");
      case GREATER:
        checkNumberOperands(operator, left, right);
        return (double)left > (double)right;
      case GREATER_EQUAL:
        checkNumberOperands(operator, left, right);
        return (double)left >= (double)right;
      case LESS:
        checkNumberOperands(operator, left, right);
        return (double)left < (double)right;
      case LESS_EQUAL:
        checkNumberOperands(operator, left, right);
        return (double)left <= (double)right;
      case BANG_EQUAL:
        checkNumberOperands(operator, left, right);
        return !isEqual(left, right);
      case EQUAL_EQUAL:
        checkNumberOperands(operator, left, right);
        return  isEqual(left, right);
      default:
        // Binary expression should only take supported operators. Cannot reach here.
        break;
    }

    return null;
  }

  @Override
  public Object visit(Grouping expr) {
    return evaluate(expr.getExpr());
  }

  @Override
  public Object visit(Literal expr) {
    return expr.getValue();
  }

  @Override
  public Object visit(Unary expr) {
    Object right = evaluate(expr.getRight());
    Token operator = expr.getOperator();

    switch (operator.getType()) {
      case BANG:
        return !isTruthy(right);
      case MINUS:
        checkNumberOperands(operator, right);
        return -(double)right;
      default:
        // Unary expression should only take either ! or -. Cannot reach here.
        break;
    }

    return null;
  }

  @Override
  public Object visit(Variable expr) {
    return environment.get(expr.getName());
  }

  @Override
  public Object visit(Assign expr) {
    Object value = evaluate(expr.getValue());
    environment.assign(expr.getName(), value);
    return value;
  }

  @Override
  public Object visit(Logical expr) {
    Object left = evaluate(expr.getLeft());
    if (expr.getOperator().getType() == TokenType.OR) {
      if (isTruthy(left))
        return left;
    } else {
      if (!isTruthy(left))
        return left;
    }

    return evaluate(expr.getRight());
  }

  @Override
  public Object visit(Call expr) {
    Object callee = evaluate(expr.getCallee());

    List<Object> arguments = new ArrayList<>();
    for (var arg : expr.getArguments())
      arguments.add(evaluate(arg));

    if (!(callee instanceof LoxCallable))
      throw new RuntimeError(expr.getToken(), "Can only call functions and classes.");

    LoxCallable function = (LoxCallable)callee;
    if (function.arity() != arguments.size())
      throw new RuntimeError(expr.getToken(),
                     "Expected " + function.arity() + " arguments but got "
                             + arguments.size() + ".");

    return function.call(this, arguments);
  }

  @Override
  public Void visit(Print stmt) {
    Object value = evaluate(stmt.getExpression());
    System.out.println(stringfy(value));
    return null;
  }

  @Override
  public Void visit(Expression stmt) {
    evaluate(stmt.getExpression());
    return null;
  }

  @Override
  public Void visit(Var stmt) {
    Object value = null;
    if (stmt.getInitializer() != null)
      value = evaluate(stmt.getInitializer());

    environment.define(stmt.getName().getLexeme(), value);
    return null;
  }

  @Override
  public Void visit(Block stmt) {
    execute(stmt.getStatements(), new Environment(environment));
    return null;
  }

  @Override
  public Void visit(If stmt) {
    if (isTruthy(evaluate(stmt.getCondition())))
      execute(stmt.getThenBranch());
    else if (stmt.getElseBranch() != null)
      execute(stmt.getElseBranch());

    return null;
  }

  @Override
  public Void visit(While stmt) {
    while (isTruthy(evaluate(stmt.getCondition())))
      execute(stmt.getBody());

    return null;
  }

  @Override
  public Void visit(Function stmt) {
    LoxFunction function = new LoxFunction(stmt);
    environment.define(stmt.getName().getLexeme(), function);

    return null;
  }

  @Override
  public Void visit(Return stmt) {
    Object value = null;
    if (stmt.getValue() != null)
      value = evaluate(stmt.getValue());

    throw new ReturnValueException(value);
  }

  private Object evaluate(Expr expr) {
    return expr.accept(this);
  }

  private void execute(Stmt stmt) {
    stmt.accept(this);
  }

  void execute(List<Stmt> statements, Environment current) {
    Environment previous = environment;

    try {
      environment = current;
      for (Stmt statement : statements)
        execute(statement);
    } finally {
      environment = previous;
    }
  }

  private boolean isTruthy(Object o) {
    if (o == null)
      return false;
    if (o instanceof Boolean)
      return (boolean)o;
    return true;
  }

  private boolean isEqual(Object a, Object b) {
    if (a == null && b == null)
      return true;
    if (a == null)
      return false;

    return a.equals(b);
  }

  private void checkNumberOperands(Token operator, Object... operands) {
    for (var operand : operands) {
      if (!(operand instanceof Double))
        throw new RuntimeError(operator, "Operands must be numbers.");
    }
  }

  private String stringfy(Object o) {
    if (o == null)
      return "nil";

    if (o instanceof Double) {
      String text = o.toString();
      if (text.endsWith(".0"))
        text = text.substring(0, text.length() - 2);

      return text;
    }

    return o.toString();
  }
}
