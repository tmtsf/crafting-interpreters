package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.exception.RuntimeError;
import com.github.tmtsf.lox.scanner.Token;

import java.util.HashMap;
import java.util.Map;

class Environment {
  private final Environment enclosing;
  private final Map<String, Object> values = new HashMap<>();

  Environment() {
    enclosing = null;
  }

  Environment(Environment enclosing) {
    this.enclosing = enclosing;
  }

  void define(String name, Object object) {
    values.put(name, object);
  }

  Object get(Token name) {
    if (values.containsKey(name.getLexeme()))
      return values.get(name.getLexeme());

    if (enclosing != null)
      return enclosing.get(name);

    throw new RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
  }

  Object getAt(Integer distance, String name) {
    return ancestor(distance).values.get(name);
  }

  private Environment ancestor(int distance) {
    var result = this;
    for (int i = 0; i < distance; ++i)
      result = result.enclosing;

    return result;
  }

  void assign(Token name, Object value) {
    if (values.containsKey(name.getLexeme())) {
      values.put(name.getLexeme(), value);
      return;
    }

    if (enclosing != null) {
      enclosing.assign(name, value);
      return;
    }

    throw new RuntimeError(name, "Undefined variable '" + name.getLexeme() + "'.");
  }

  void assignAt(int distance, Token token, Object value) {
    ancestor(distance).values.put(token.getLexeme(), value);
  }
}
