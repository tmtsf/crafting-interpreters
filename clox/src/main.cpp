#include "vm.hpp"
#include "chunk.hpp"

int main(int argc, const char* argv[]) {
  clox::vm::VM vm;

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