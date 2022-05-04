package com.github.tmtsf.lox.ast.stmt;

import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.visitor.StmtVisitor;

import java.util.List;

public class Function extends Stmt {
  private final Token token;
  private final List<Token> parameters;
  private final List<Stmt> body;

  public Function(Token token, List<Token> parameters, List<Stmt> body) {
    this.token = token;
    this.parameters = parameters;
    this.body = body;
  }

  public Token getToken() {
    return token;
  }

  public List<Token> getParameters() {
    return parameters;
  }

  public List<Stmt> getBody() {
    return body;
  }

  @Override
  public <R> R accept(StmtVisitor<R> visitor) {
    return visitor.visit(this);
  }
}
