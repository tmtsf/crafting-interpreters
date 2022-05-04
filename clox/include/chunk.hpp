#pragma once

#include "common.hpp"

namespace clox {
  namespace vm {
    enum class OpCode {
      OP_CONSTANT,
      OP_RETURN,
      OP_NEGATE,
      OP_ADD,
      OP_SUBTRACT,
      OP_MULTIPLY,
      OP_DIVIDE,
    };

    class Chunk {
    public:
      void write(const byte_code_t& code,
                 int line);
      size_t addConstant(const value_t& value);
      void disassemble(const std::string& name) const;

      const byte_code_vec_t& getByteCodes(void) const;
      const value_vec_t& getConstants(void) const;
      void printValue(const value_t& value) const;
    private:
      size_t disassemble(size_t offset) const;

      size_t simpleInstruction(const std::string& name, size_t offset) const;
      size_t constantInstruction(const std::string& name, size_t offset) const;
    private:
      byte_code_vec_t m_ByteCodes;
      value_vec_t m_Constants;
      int_vec_t m_Lines;
    };
  }
}
