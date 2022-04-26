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
    byte[] bytes = Files.readAllBytes(Paths.get(testDir + filename));

    Scanner scanner = new Scanner(new String(bytes, Charset.defaultCharset()));
    var tokens = scanner.scanTokens();

    Parser parer = new Parser(tokens);
    interpreter.interpret(parer.parse());
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
}
