#include "vm.hpp"
#include "chunk.hpp"
#include "common.hpp"
#include <iostream>

namespace {
  void repl(clox::vm::VM& vm) {
    clox::string_t line;
    for ( ; ; ) {
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

  //size_t constant = vm.addConstant(1.2);
  //vm.write(clox::vm::OpCode::CONSTANT, 123);
  //vm.write(constant, 123);

  //constant = vm.addConstant(3.4);
  //vm.write(clox::vm::OpCode::CONSTANT, 123);
  //vm.write(constant, 123);

  //vm.write(clox::vm::OpCode::ADD, 123);

  //constant = vm.addConstant(5.6);
  //vm.write(clox::vm::OpCode::CONSTANT, 123);
  //vm.write(constant, 123);

  //vm.write(clox::vm::OpCode::DIVIDE, 123);
  //vm.write(clox::vm::OpCode::NEGATE, 123);

  //vm.write(clox::vm::OpCode::RETURN, 123);

  vm.interpret("3 + 4 * 5 - 81 / 9");

  printf("\n");
  vm.disassemble("test chunk");

  return 0;
}