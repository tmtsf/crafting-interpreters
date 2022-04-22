package com.github.tmtsf.lox.exception;

import com.github.tmtsf.lox.scanner.Token;

public class RuntimeError extends RuntimeException {
  private final Token token;

  public RuntimeError(Token token, String message) {
    super(message);
    this.token = token;
  }

  public Token getToken() {
    return token;
  }
}
