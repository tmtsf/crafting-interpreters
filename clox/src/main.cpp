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

  vm.interpret("!(5 - 4 > 3 * 2 == !nil)");

  return 0;
}