package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.Lox;
import com.github.tmtsf.lox.ast.expr.*;
import com.github.tmtsf.lox.exception.RuntimeError;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.ExprVisitor;

public class Interpreter implements ExprVisitor<Object> {
  public void interpret(Expr expr) {
    try {
      Object value = evaluate(expr);
      System.out.println(stringfy(value));
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
    }

    return null;
  }

  private Object evaluate(Expr expr) {
    return expr.accept(this);
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
