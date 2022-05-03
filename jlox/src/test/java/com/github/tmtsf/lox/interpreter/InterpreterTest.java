package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.parser.Parser;
import com.github.tmtsf.lox.scanner.Scanner;
import org.junit.jupiter.api.Test;

import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;

public class InterpreterTest {
  private static final String testDir = System.getProperty("user.dir") + "\\..\\test\\";
  private static final Interpreter interpreter = new Interpreter();

  private void doTest(String filename) throws Exception {
    var bytes = Files.readAllBytes(Paths.get(testDir + filename));

    var scanner = new Scanner(new String(bytes, Charset.defaultCharset()));
    var tokens = scanner.scanTokens();

    var parer = new Parser(tokens);
    var statements = parer.parse();

    var resolver = new Resolver(interpreter);
    resolver.resolve(statements);

    interpreter.interpret(statements);
  }

  @Test
  public void testExpression() throws Exception {
    doTest("expression.lox");
  }

  @Test
  public void testStatement() throws Exception {
    doTest("statement.lox");
  }

  @Test
  public void testScope() throws Exception {
    doTest("scope.lox");
  }

  @Test
  public void testIf() throws Exception {
    doTest("if.lox");
  }

  @Test
  public void testLogical() throws Exception {
    doTest("logical.lox");
  }

  @Test
  public void testWhile() throws Exception {
    doTest("while.lox");
  }

  @Test
  public void testFor() throws Exception {
    doTest("for.lox");
  }

  @Test
  public void testFunArity() throws Exception {
    doTest("fun_arity.lox");
  }

  @Test
  public void testFunCount() throws Exception {
    doTest("fun_count.lox");
  }

  @Test
  public void testFunHi() throws Exception {
    doTest("fun_hi.lox");
  }

  @Test
  public void testFunName() throws Exception {
    doTest("fun_name.lox");
  }

  @Test
  public void testFib() throws Exception {
    doTest("fib.lox");
  }

  @Test
  public void testLocal() throws Exception {
    doTest("local.lox");
  }

  @Test
  public void testLocalError() throws Exception {
    doTest("local_err.lox");
  }
}
