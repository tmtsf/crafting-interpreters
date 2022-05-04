package com.github.tmtsf.lox;

import com.github.tmtsf.lox.ast.stmt.Stmt;
import com.github.tmtsf.lox.exception.RuntimeError;
import com.github.tmtsf.lox.interpreter.Interpreter;
import com.github.tmtsf.lox.interpreter.Resolver;
import com.github.tmtsf.lox.parser.Parser;
import com.github.tmtsf.lox.scanner.Token;
import com.github.tmtsf.lox.scanner.Scanner;
import com.github.tmtsf.lox.scanner.TokenType;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

public class Lox {
  static boolean hadError = false;
  static boolean hadRuntimeError = false;

  private static final Interpreter interpreter = new Interpreter();

  public static void main(String[] args) throws IOException {
    if (args.length > 1) {
      System.out.println("Usage: jlox [script]");
      System.exit(64);
    } else if (args.length == 1) {
      runFile(args[0]);
    } else {
      runPrompt();
    }
  }

  private static void runFile(String path) throws IOException {
    byte[] bytes = Files.readAllBytes(Paths.get(path));
    run(new String(bytes, Charset.defaultCharset()));

    if (hadError)
      System.exit(65);

    if (hadRuntimeError)
      System.exit(70);
  }

  private static void runPrompt() throws IOException {
    InputStreamReader input = new InputStreamReader(System.in);
    BufferedReader reader = new BufferedReader(input);

    for ( ; ; ) {
      System.out.print("> ");
      String line = reader.readLine();
      if (line == null) break;
      run(line);
      hadError = false;
    }
  }

  private static void run(String source) {
    Scanner scanner = new Scanner(source);
    List<Token> tokens = scanner.scanTokens();
    Parser parser = new Parser(tokens);
    List<Stmt> statements = parser.parse();

    if (hadError)
      return;

    // System.out.println(new ASTPrinter().print(expr));

    Resolver resolver = new Resolver(interpreter);
    resolver.resolve(statements);

    interpreter.interpret(statements);
  }

  public static void error(int line, String message) {
    report(line, "", message);
  }

  public static void error(Token token, String message) {
    if (token.getType() == TokenType.EOF)
      report(token.getLine(), " at end", message);
    else
      report(token.getLine(), " at '" + token.getLexeme() + "'", message);
  }

  private static void report(int line, String where, String message) {
    System.err.println("[line " + line + "] Error " + where + ": " + message);
    hadError = true;
  }

  public static void runtimeError(RuntimeError e) {
    System.err.println(e.getMessage() + "\n[line " + e.getToken().getLine() + "]: " + e.getToken().getLexeme());
    hadRuntimeError = true;
  }
}
