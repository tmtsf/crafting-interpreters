#include "vm.hpp"
#include "chunk.hpp"
#include "compiler.hpp"

namespace clox {
  namespace vm {
    VM::VM(void) :
      m_Chunk(std::make_shared<Chunk>()),
      m_IP(0)
    { }

    VM::VM(const chunk_ptr_t& chunk) :
      m_Chunk(chunk),
      m_IP(0)
    { }

    void VM::write(const byte_code_t& code,
                   int line) {
      m_Chunk->write(code, line);
    }

    size_t VM::addConstant(const value_t& value) {
      return m_Chunk->addConstant(value);
    }

    void VM::disassemble(const std::string& name) const {
      m_Chunk->disassemble(name);
    }

    InterpretResult VM::interpret(void) {
      for ( ; ; ) {
        OpCode instruction;
        switch (instruction = readByte()) {
        case OpCode::OP_CONSTANT: {
          auto value = readConstant();
          m_Stack.push(value);
          break;
        }
        case OpCode::OP_NEGATE: {
          auto value = m_Stack.top();
          m_Stack.pop();
          m_Stack.push(-value);
          break;
        }
        case OpCode::OP_ADD:
          binaryOp('+');
          break;
        case OpCode::OP_SUBTRACT:
          binaryOp('-');
          break;
        case OpCode::OP_MULTIPLY:
          binaryOp('*');
          break;
        case OpCode::OP_DIVIDE:
          binaryOp('/');
          break;
        case OpCode::OP_RETURN:
          m_Chunk->printValue(m_Stack.top());
          m_Stack.pop();
          printf("\n");
          return InterpretResult::INTERPRET_OK;
        default:
          return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
      }
    }

    InterpretResult VM::interpret(const std::string& source) {
      clox::compiler::Compiler::compile(source);
      return InterpretResult::INTERPRET_OK;
    }

    const OpCode& VM::readByte(void) {
      const byte_code_vec_t& byteCodes = m_Chunk->getByteCodes();
      return std::get<OpCode>(byteCodes[m_IP++]);
    }

    const value_t& VM::readConstant(void) {
      const byte_code_vec_t& byteCodes = m_Chunk->getByteCodes();
      size_t offset = std::get<size_t>(byteCodes[m_IP++]);

      const value_vec_t& constants = m_Chunk->getConstants();
      return constants[offset];
    }

    void VM::binaryOp(char c) {
      value_t right = m_Stack.top();
      m_Stack.pop();
      value_t left = m_Stack.top();
      m_Stack.pop();

      switch (c) {
      case '+':
        m_Stack.push(left + right);
        break;
      case '-':
        m_Stack.push(left - right);
        break;
      case '*':
        m_Stack.push(left * right);
        break;
      case '/':
        m_Stack.push(left / right);
        break;
      default:
        throw("Unknown binary operator");
        break;
      }
    }
  }
}