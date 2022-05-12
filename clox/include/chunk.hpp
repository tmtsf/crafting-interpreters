#pragma once

#include "common.hpp"

namespace clox {
  namespace vm {
    enum class OpCode {
      CONSTANT,
      RETURN,
      NEGATE,
      ADD,
      SUBTRACT,
      MULTIPLY,
      DIVIDE,
      NIL,
      TRUE,
      FALSE,
      NOT,
      EQUAL,
      GREATER,
      LESS
    };

    class Chunk {
    public:
      void write(const byte_code_t& code,
                 int_t line);
      size_t addConstant(const value_t& value);
      void disassemble(const string_t& name) const;

      const byte_code_vec_t& getByteCodes(void) const;
      const value_vec_t& getConstants(void) const;
      void printValue(const value_t& value) const;
    private:
      size_t disassemble(size_t offset) const;

      size_t simpleInstruction(const string_t& name, size_t offset) const;
      size_t constantInstruction(const string_t& name, size_t offset) const;
    private:
      byte_code_vec_t m_ByteCodes;
      value_vec_t m_Constants;
      int_vec_t m_Lines;
    };
  }
}
