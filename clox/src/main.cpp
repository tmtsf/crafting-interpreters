#include "vm.hpp"
#include "chunk.hpp"
#include <iostream>

namespace {
  void repl(clox::vm::VM& vm) {
    std::string line;
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

  if (argc == 1) {
    repl(vm);
  }
  else if (argc == 2) {
    runFile(vm, argv[1]);
  }
  else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  size_t constant = vm.addConstant(1.2);
  vm.write(clox::vm::OpCode::OP_CONSTANT, 123);
  vm.write(constant, 123);

  constant = vm.addConstant(3.4);
  vm.write(clox::vm::OpCode::OP_CONSTANT, 123);
  vm.write(constant, 123);

  vm.write(clox::vm::OpCode::OP_ADD, 123);

  constant = vm.addConstant(5.6);
  vm.write(clox::vm::OpCode::OP_CONSTANT, 123);
  vm.write(constant, 123);

  vm.write(clox::vm::OpCode::OP_DIVIDE, 123);
  vm.write(clox::vm::OpCode::OP_NEGATE, 123);

  vm.write(clox::vm::OpCode::OP_RETURN, 123);

  vm.interpret();

  printf("\n");
  vm.disassemble("test chunk");

  return 0;
}