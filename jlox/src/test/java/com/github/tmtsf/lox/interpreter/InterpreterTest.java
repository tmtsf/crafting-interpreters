package com.github.tmtsf.lox.interpreter;

import com.github.tmtsf.lox.parser.Parser;
import com.github.tmtsf.lox.scanner.Scanner;
import org.junit.jupiter.api.Test;

import java.io.File;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;

public class InterpreterTest {
  private static final String testDir = System.getProperty("user.dir") + "\\..\\test\\";
  private static final Interpreter interpreter = new Interpreter();

  private void testDirectory(String directory) throws Exception {
    File dir = new File(directory);
    for (var item : dir.list()) {
      File file = new File(dir, item);
      if (file.isDirectory()) {
        System.out.println("Directory: " + directory + item + "\\");
        testDirectory(directory + item + "\\");
      } else {
        System.out.println("File: " + directory + item);
        testFile(directory + item);
      }
    }
  }

  private void testFile(String filename) throws Exception {
    var bytes = Files.readAllBytes(Paths.get(filename));

    var scanner = new Scanner(new String(bytes, Charset.defaultCharset()));
    var tokens = scanner.scanTokens();

    var parer = new Parser(tokens);
    var statements = parer.parse();

    var resolver = new Resolver(interpreter);
    resolver.resolve(statements);

    interpreter.interpret(statements);
  }

  @Test
  public void test() throws Exception {
    testDirectory(testDir);
  }
}
