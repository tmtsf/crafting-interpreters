package com.github.tmtsf.lox.exception;

public class ReturnValueException extends RuntimeException {
  private final Object value;

  public ReturnValueException(Object value) {
    super(null, null, false, false);
    this.value = value;
  }

  public Object getValue() {
    return value;
  }
}
