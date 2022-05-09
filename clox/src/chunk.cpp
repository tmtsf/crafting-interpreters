#include "chunk.hpp"

namespace clox {
  namespace vm {

    void Chunk::write(const byte_code_t& code,
                      int line) {
      m_ByteCodes.push_back(code);
      m_Lines.push_back(line);
    }

    size_t Chunk::addConstant(const value_t& value) {
      m_Constants.push_back(value);
      return m_Constants.size() - 1;
    }

    void Chunk::disassemble(const string_t& name) const {
      printf("== %s ==\n", name.c_str());

      for (size_t offset = 0; offset < m_ByteCodes.size(); )
        offset = disassemble(offset);
    }

    const byte_code_vec_t& Chunk::getByteCodes(void) const {
      return m_ByteCodes;
    }

    const value_vec_t& Chunk::getConstants(void) const {
      return m_Constants;
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
      case OpCode::CONSTANT:
        return constantInstruction("CONSTANT", offset);
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

    void Chunk::printValue(const value_t& value) const {
      printf("%g", value);
    }
  }
}