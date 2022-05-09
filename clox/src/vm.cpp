#include "vm.hpp"
#include "chunk.hpp"
#include "compiler.hpp"

namespace clox {
  using namespace compiler;

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

    void VM::disassemble(const string_t& name) const {
      m_Chunk->disassemble(name);
    }

    InterpretResult VM::interpret(void) {
      return run();
    }

    InterpretResult VM::interpret(const string_t& source) {
      Compiler compiler(m_Chunk);
      if (!compiler.compile(source))
        return InterpretResult::COMPILE_ERROR;

      return run();
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
      value_t right = m_Stack.back();
      m_Stack.pop_back();
      value_t left = m_Stack.back();
      m_Stack.pop_back();

      switch (c) {
      case '+':
        m_Stack.push_back(left + right);
        break;
      case '-':
        m_Stack.push_back(left - right);
        break;
      case '*':
        m_Stack.push_back(left * right);
        break;
      case '/':
        m_Stack.push_back(left / right);
        break;
      default:
        throw("Unknown binary operator");
        break;
      }
    }

    InterpretResult VM::run(void) {
      for (; ; ) {
        OpCode instruction;
        switch (instruction = readByte()) {
        case OpCode::CONSTANT: {
          auto value = readConstant();
          m_Stack.push_back(value);
          break;
        }
        case OpCode::NEGATE: {
          auto value = m_Stack.back();
          m_Stack.pop_back();
          m_Stack.push_back(-value);
          break;
        }
        case OpCode::ADD:
          binaryOp('+');
          break;
        case OpCode::SUBTRACT:
          binaryOp('-');
          break;
        case OpCode::MULTIPLY:
          binaryOp('*');
          break;
        case OpCode::DIVIDE:
          binaryOp('/');
          break;
        case OpCode::RETURN:
          m_Chunk->printValue(m_Stack.back());
          m_Stack.pop_back();
          printf("\n");
          return InterpretResult::OK;
        default:
          return InterpretResult::RUNTIME_ERROR;
        }
      }
    }
  }
}