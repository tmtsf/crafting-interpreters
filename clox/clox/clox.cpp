#include "vm.hpp"
#include "chunk.hpp"
#include "common.hpp"
#include <iostream>

namespace {
  void repl(clox::vm::VM& vm) {
    clox::string_t line;
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

  void runFile(const clox::vm::VM& vm, const char* path) {

  }
}

int main(int argc, const char* argv[]) {
  clox::vm::VM vm;

  //if (argc == 1) {
  //  repl(vm);
  //}
  //else if (argc == 2) {
  //  runFile(vm, argv[1]);
  //}
  //else {
  //  fprintf(stderr, "Usage: clox [path]\n");
  //  exit(64);
  //}

  //vm.interpret("var a = 3; \n"
  //             "{ \n"
  //             "  var b = a; \n"
  //             "  print b + 1; \n"
  //             "  b = a * 4; \n"
  //             "  { \n"
  //             "    var c = a * a; \n"
  //             "    var d = c + b; \n"
  //             "    print d * 2; \n"
  //             "  } \n"
  //             "}");

  //vm.interpret("var a = 5;\n"
  //             "if (a > 4)\n"
  //             "  print \"Larger than 4\";"
  //             "else\n"
  //             "  print \"Less than 4\";\n"
  //             "print a + 1;");

  //vm.interpret("var a = 5;\n"
  //             "while (a < 10) {\n"
  //             "  print a;\n"
  //             "  a = a + 1;\n"
  //             "}");

  vm.interpret("for (var a = 5; a < 10; a = a + 1) {\n"
               "  print a;\n"
               "}\n");

  return 0;
} 