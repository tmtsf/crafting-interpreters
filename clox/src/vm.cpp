#include "vm.hpp"
#include "chunk.hpp"
#include "compiler.hpp"
#include "object.hpp"

namespace clox {
  using namespace compiler;
  using namespace obj;

  namespace vm {
    VM::VM(void) :
      m_Chunk(std::make_shared<Chunk>()),
      m_IP(0)
    { }

    VM::VM(const chunk_ptr_t& chunk) :
      m_Chunk(chunk),
      m_IP(0)
    { }

    InterpretResult VM::interpret(void) {
      return run();
    }

    InterpretResult VM::interpret(const string_t& source) {
      Compiler compiler(m_Chunk);
      if (!compiler.compile(source))
        return InterpretResult::COMPILE_ERROR;

      m_Chunk->disassemble("debug chunk");
      printf("\n\n");

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

    void VM::binaryAdd(void) {
      value_t r = peek(0);
      value_t l = peek(1);

      m_Stack.pop_back();
      m_Stack.pop_back();

      if (std::holds_alternative<dbl_t>(r) || std::holds_alternative<dbl_t>(l)) {
        dbl_t right = std::get<dbl_t>(r);
        dbl_t left = std::get<dbl_t>(l);
        m_Stack.push_back(left + right);
        return;
      }
      else if (std::holds_alternative<obj_ptr_t>(r) || std::holds_alternative<obj_ptr_t>(l)) {
        auto pL = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(l));
        auto pR = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(r));
        if (pL && pR)
          m_Stack.push_back(Object::formStringObject(pL->getString() + pR->getString()));

        return;
      }

      fprintf(stderr, "Operands for '+' must be either numbers or strings.");
      throw;
    }

    void VM::binaryOp(char c) {
      const auto& r = peek(0);
      const auto& l = peek(1);
      if (!std::holds_alternative<dbl_t>(r) || !std::holds_alternative<dbl_t>(l)) {
        fprintf(stderr, "Operands must be numbers.");
        throw;
      }

      dbl_t right = std::get<dbl_t>(r);
      dbl_t left = std::get<dbl_t>(l);
      m_Stack.pop_back();
      m_Stack.pop_back();

      switch (c) {
      case '+':
        binaryAdd();
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
      case '>':
        m_Stack.push_back(left > right);
        break;
      case '<':
        m_Stack.push_back(left < right);
        break;
      default:
        throw("Unknown binary operator");
        break;
      }
    }

    InterpretResult VM::run(void) {
      while (true) {
        OpCode instruction;
        switch (instruction = readByte()) {
        case OpCode::CONSTANT: {
          const value_t& value = readConstant();
          m_Stack.push_back(value);
          break;
        }
        case OpCode::NEGATE: {
          dbl_t value = -std::get<dbl_t>(peek(0));
          m_Stack.pop_back();
          m_Stack.push_back(value);
          break;
        }
        case OpCode::ADD:
          binaryAdd();
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
        case OpCode::NIL:
          m_Stack.push_back(nullptr);
          break;
        case OpCode::FALSE:
          m_Stack.push_back(false);
          break;
        case OpCode::TRUE:
          m_Stack.push_back(true);
          break;
        case OpCode::NOT: {
          bool value = isFalsey(peek(0));
          m_Stack.pop_back();
          m_Stack.push_back(value);
          break;
        }
        case OpCode::EQUAL: {
          const value_t& right = peek(0);
          const value_t& left = peek(1);
          bool value = areEqual(left, right);
          m_Stack.pop_back();
          m_Stack.pop_back();
          m_Stack.push_back(value);
          break;
        }
        case OpCode::GREATER:
          binaryOp('>');
          break;
        case OpCode::LESS:
          binaryOp('<');
          break;
        default:
          return InterpretResult::RUNTIME_ERROR;
        }
      }
    }

    const value_t& VM::peek(size_t offset) const {
      return *(m_Stack.crbegin() + offset);
    }

    bool VM::isFalsey(const value_t& value) const {
      return std::visit(
        util::overloaded{
          [](dbl_t x) { return x != 0.; },
          [](bool x) { return !x; },
          [](nullptr_t x) { return true; }
        },
        value
      );
    }

    bool VM::areEqual(const value_t& left, const value_t& right) const {
      if (left.index() != right.index())
        return false;

      if (std::holds_alternative<bool>(left))
        return std::get<bool>(left) == std::get<bool>(right);
      else if (std::holds_alternative<dbl_t>(left))
        return std::get<dbl_t>(left) == std::get<dbl_t>(right);
      else if (std::holds_alternative<nullptr_t>(left))
        return true;
      else if (std::holds_alternative<obj_ptr_t>(left)) {
        auto pL = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(left));
        auto pR = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(right));
        if (pL && pR)
          return pL->getString().compare(pR->getString()) == 0;
      }

      return false;
    }
  }
}