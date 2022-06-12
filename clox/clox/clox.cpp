#include "vm.hpp"
#include "chunk.hpp"
#include "common.hpp"
#include <iostream>
#include <fstream>

namespace {
  void repl(clox::vm::VM& vm) {
    std::string line;
    while (true) {
      printf("> ");

      std::getline(std::cin, line);
      if (line.empty()) {
        printf("\n");
        break;
      }

      vm.interpret(line);
    }
  }

  void runFile(clox::vm::VM& vm, const char* path) {
    std::string source;
    std::string line;

    std::ifstream in(path);
    while (std::getline(in, line)) {
      source += line;
      source += "\n";
    }

    vm.interpret(source);
  }
}

int main(int argc, const char* argv[]) {
  clox::vm::VM vm;

  //if (argc == 1) {
  //  repl(vm);
  //} else if (argc == 2) {
  //  runFile(vm, argv[1]);
  //} else {
  //  fprintf(stderr, "Usage: clox [path]\n");
  //  exit(64);
  //}

  //vm.interpret("var a = 3;                   \n"
  //             "var b = 4;                   \n"
  //             "{                            \n"
  //             "  var d = 5;                 \n"
  //             "  print a + b * d;           \n"
  //             "}                            \n");

  //vm.interpret("fun hello() {                \n"
  //             "  print \"Hello!\";          \n"
  //             "}                            \n"
  //             "print hello;                 \n");

  //vm.interpret("fun sum(a, b, c) {           \n"
  //             "  return a + b + c;          \n"
  //             "}                            \n"
  //             "print 4 + sum(5, 6, 7);      \n");

  //vm.interpret("var x = \"global\";          \n"
  //             "fun outer() {                \n"
  //             "  var x = \"outer\";         \n"
  //             "  fun inner() {              \n"
  //             "    print x;                 \n"
  //             "  }                          \n"
  //             "  inner();                   \n"
  //             "}                            \n"
  //             "outer();                     \n");

  //vm.interpret("fun noReturn() {             \n"
  //             "  print \"Do something.\";   \n"
  //             "}                            \n"
  //             "print noReturn();            \n");

  //vm.interpret("var x = 123;                 \n"
  //             "x();                         \n");

  vm.interpret("{                            \n"
               "  fun fib(n) {               \n"
               "    if (n < 2) return n;     \n"
               "    return fib(n - 1) + fib(n - 2);   \n"
               "  }                                   \n"
               "  print fib(8);                       \n"
               "}                                     \n");

  return 0;
} 