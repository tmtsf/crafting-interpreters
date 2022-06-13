#include "chunk.hpp"
#include "object.hpp"

namespace clox {
  namespace vm {

    void Chunk::write(const byte_code_t& code, int_t line) {
      m_ByteCodes.push_back(code);
      m_Lines.push_back(line);
    }

    void Chunk::replace(size_t offset, const byte_code_t& code) {
      m_ByteCodes[offset] = code;
    }

    size_t Chunk::addConstant(const value_t& value) {
      m_Constants.push_back(value);
      return m_Constants.size() - 1;
    }

    size_t Chunk::size(void) const {
      return m_ByteCodes.size();
    }

    void Chunk::disassemble(const string_t& name) const {
      printf("== %s ==\n", name.c_str());

      for (size_t offset = 0; offset < m_ByteCodes.size(); )
        offset = disassemble(offset);
    }

    const OpCode& Chunk::readByte(size_t offset) {
      return std::get<OpCode>(m_ByteCodes[offset]);
    }

    size_t Chunk::readOffset(size_t offset) {
      return std::get<size_t>(m_ByteCodes[offset]);
    }

    const value_t& Chunk::readConstant(size_t offset) {
      return m_Constants[offset];
    }

    size_t Chunk::disassemble(size_t offset) const {
      printf("%04zu ", offset);

      if (offset > 0 && m_Lines[offset] == m_Lines[offset - 1])
        printf("   | ");
      else
        printf("%4d ", m_Lines[offset]);

      auto instruction = std::get<OpCode>(m_ByteCodes[offset]);
      switch (instruction) {
      case OpCode::RETURN:
        return simpleInstruction("RETURN", offset);
      case OpCode::PRINT:
        return simpleInstruction("PRINT", offset);
      case OpCode::POP:
        return simpleInstruction("POP", offset);
      case OpCode::NEGATE:
        return simpleInstruction("NEGATE", offset);
      case OpCode::ADD:
        return simpleInstruction("ADD", offset);
      case OpCode::SUBTRACT:
        return simpleInstruction("SUBTRACT", offset);
      case OpCode::MULTIPLY:
        return simpleInstruction("MULTIPLY", offset);
      case OpCode::DIVIDE:
        return simpleInstruction("DIVIDE", offset);
      case OpCode::NIL:
        return simpleInstruction("NIL", offset);
      case OpCode::FALSE:
        return simpleInstruction("FALSE", offset);
      case OpCode::TRUE:
        return simpleInstruction("TRUE", offset);
      case OpCode::CONSTANT:
        return constantInstruction("CONSTANT", offset);
      case OpCode::NOT:
        return simpleInstruction("NOT", offset);
      case OpCode::EQUAL:
        return simpleInstruction("EQUAL", offset);
      case OpCode::GREATER:
        return simpleInstruction("GREATER", offset);
      case OpCode::LESS:
        return simpleInstruction("LESS", offset);
      case OpCode::DEFINE_GLOBAL:
        return constantInstruction("DEFINE_GLOBAL", offset);
      case OpCode::GET_GLOBAL:
        return constantInstruction("GET_GLOBAL", offset);
      case OpCode::SET_GLOBAL:
        return constantInstruction("SET_GLOBAL", offset);
      case OpCode::GET_LOCAL:
        return byteInstruction("GET_LOCAL", offset);
      case OpCode::SET_LOCAL:
        return byteInstruction("SET_LOCAL", offset);
      case OpCode::JUMP_IF_FALSE:
        return jumpInstruction("JUMP_IF_FALSE", 1, offset);
      case OpCode::JUMP:
        return jumpInstruction("JUMP", 1, offset);
      case OpCode::LOOP:
        return jumpInstruction("LOOP", -1, offset);
      case OpCode::CALL:
        return byteInstruction("CALL", offset);
      case OpCode::CLOSURE:
      {
        ++offset;
        size_t constant = std::get<size_t>(m_ByteCodes[offset++]);
        printf("%-16s %4zu ", "CLOSURE", constant);
        printValue(m_Constants[constant]);
        printf("\n");
        return offset;
      }
      default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
      }
    }

    size_t Chunk::simpleInstruction(const string_t& name, size_t offset) const {
      printf("%s\n", name.c_str());
      return offset + 1;
    }

    size_t Chunk::constantInstruction(const string_t& name, size_t offset) const {
      auto constant = std::get<size_t>(m_ByteCodes[offset + 1]);
      printf("%-16s %4zu '", name.c_str(), constant);
      printValue(m_Constants[constant]);
      printf("'\n");

      return offset + 2;
    }

    size_t Chunk::byteInstruction(const string_t& name, size_t offset) const {
      auto constant = std::get<size_t>(m_ByteCodes[offset + 1]);
      printf("%-16s %4zu\n", name.c_str(), constant);

      return offset + 2;
    }

    size_t Chunk::jumpInstruction(const string_t& name, int_t sign, size_t offset) const {
      auto jump = std::get<size_t>(m_ByteCodes[offset + 1]);
      printf("%-16s %4zu -> %zu\n", name.c_str(), offset, offset + 2 + sign * jump);

      return offset + 2;
    }

    // todo: use visitor pattern to print for specific types
    void Chunk::printValue(const value_t& value) const {
      std::visit(
        util::overloaded(
          [](dbl_t x) { printf("%g", x);  },
          [](bool x) { printf("%s", x ? "true" : "false"); },
          [](nullptr_t x) { printf("nil"); },
          [](obj_ptr_t x) { x->print(); }
        ),
        value
      );
    }
  }
}