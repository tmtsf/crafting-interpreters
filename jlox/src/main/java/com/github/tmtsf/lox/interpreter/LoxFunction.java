package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.ast.stmt.Function;
import com.github.tmtsf.lox.exception.ReturnValueException;

import java.util.List;

class LoxFunction implements LoxCallable {
  private final Function declaration;
  private final Environment closure;

  LoxFunction(Function declaration, Environment closure) {
    this.declaration = declaration;
    this.closure = closure;
  }

  @Override
  public int arity() {
    return declaration.getParameters().size();
  }

  @Override
  public Object call(Interpreter interpreter, List<Object> arguments) {
    Environment environment = new Environment(closure);
    for (int i = 0; i < declaration.getParameters().size(); ++i) {
      environment.define(declaration.getParameters().get(i).getLexeme(),
                         arguments.get(i));
    }

    try {
      interpreter.execute(declaration.getBody(), environment);
    } catch (ReturnValueException e) {
      return e.getValue();
    }
    return null;
  }

  @Override
  public String toString() {
    return "<fn " + declaration.getToken().getLexeme() + ">";
  }
}
