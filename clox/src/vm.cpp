#include "vm.hpp"
#include "chunk.hpp"
#include "compiler.hpp"
#include "object.hpp"

namespace clox {
  using namespace compiler;
  using namespace obj;

  namespace vm {
    VM::VM(void)
    { }

    InterpretResult VM::interpret(void) {
      return run();
    }

    InterpretResult VM::interpret(const string_t& source) {
      Compiler compiler;
      function_ptr_t function = compiler.compile(source);
      if (!function)
        return InterpretResult::COMPILE_ERROR;

      m_Stack.push_back(function);
      call(function, 0);

      function->m_Chunk.disassemble("debug chunk");
      printf("\n\n");

      return run();
    }

    const OpCode& VM::readByte(void) {
      CallFrame& frame = m_Frames.back();
      return frame.m_Function->m_Chunk.readByte(frame.m_IP++);
    }

    const size_t VM::readOffset(void) {
      CallFrame& frame = m_Frames.back();
      return frame.m_Function->m_Chunk.readOffset(frame.m_IP++);
    }

    const value_t& VM::readConstant(void) {
      return m_Frames.back().m_Function->m_Chunk.readConstant(readOffset());
    }

    const string_t& VM::readString(void) {
      const value_t& name = readConstant();
      const string_ptr_t& pString = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(name));
      return pString->m_Str;
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
      } else if (std::holds_alternative<obj_ptr_t>(r) || std::holds_alternative<obj_ptr_t>(l)) {
        auto pL = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(l));
        auto pR = dynamic_cast<string_ptr_t>(std::get<obj_ptr_t>(r));
        if (pL && pR)
          m_Stack.push_back(Object::formStringObject(pL->m_Str + pR->m_Str));

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
        {
          value_t result = m_Stack.back();
          m_Stack.pop_back();

          size_t top = m_Frames.back().m_Start;
          m_Frames.pop_back();
          if (m_Frames.empty()) {
            m_Stack.pop_back();
            return InterpretResult::OK;
          }
          
          m_Stack.erase(m_Stack.begin() + top, m_Stack.end());
          m_Stack.push_back(result);
          break;
        }
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
        case OpCode::PRINT:
          m_Frames.back().m_Function->m_Chunk.printValue(m_Stack.back());
          m_Stack.pop_back();
          printf("\n");
          break;
        case OpCode::POP:
          m_Stack.pop_back();
          break;
        case OpCode::DEFINE_GLOBAL:
          m_Globals[readString()] = peek(0);
          m_Stack.pop_back();
          break;
        case OpCode::GET_GLOBAL: {
          string_t name = readString();
          auto it = m_Globals.find(name);
          if (it == m_Globals.cend()) {
            fprintf(stderr, "Undefined variable '%s'.", name.c_str());
            return InterpretResult::RUNTIME_ERROR;
          }

          m_Stack.push_back(it->second);
          break;
        }
        case OpCode::SET_GLOBAL:
        {
          string_t name = readString();
          auto it = m_Globals.find(name);
          if (it == m_Globals.cend()) {
            fprintf(stderr, "Undefined variable '%s'.", name.c_str());
            return InterpretResult::RUNTIME_ERROR;
          }

          it->second = peek(0);
          break;
        }
        case OpCode::GET_LOCAL:
        {
          size_t offset = readOffset();
          value_t local = m_Stack[m_Frames.back().m_Start + offset];
          m_Stack.push_back(local);
          break;
        }
        case OpCode::SET_LOCAL:
        {
          size_t offset = readOffset();
          m_Stack[m_Frames.back().m_Start + offset] = peek(0);
          break;
        }
        case OpCode::JUMP_IF_FALSE:
        {
          size_t offset = readOffset();
          if (isFalsey(peek(0)))
            m_Frames.back().m_IP += offset;

          break;
        }
        case OpCode::JUMP:
        {
          size_t offset = readOffset();
          m_Frames.back().m_IP += offset;
          break;
        }
        case OpCode::LOOP:
        {
          size_t offset = readOffset();
          m_Frames.back().m_IP -= offset;
          break;
        }
        case OpCode::CALL:
        {
          size_t count = readOffset();
          if (!callValue(peek(count), count))
            return InterpretResult::RUNTIME_ERROR;

          break;
        }
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
          return pL->m_Str.compare(pR->m_Str) == 0;
      }

      return false;
    }

    bool VM::callValue(const value_t& value, size_t count) {
      if (std::holds_alternative<obj_ptr_t>(value)) {
        // Function
        function_ptr_t function = dynamic_cast<function_ptr_t>(std::get<obj_ptr_t>(value));
        if (function)
          return call(function, count);
      }

      fprintf(stderr, "Can only call functions now.");
      return false;
    }

    bool VM::call(const function_ptr_t& function, size_t count) {
      if (function->m_Arity != count) {
        fprintf(stderr, "Expected %zu arguments but found %zu.", function->m_Arity, count);
        return false;
      }

      m_Frames.emplace_back(function, 0, m_Stack.size() - count - 1);
      return true;
    }
  }
}