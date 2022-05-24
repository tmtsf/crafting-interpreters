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
      LESS,
      PRINT,
      POP,
      DEFINE_GLOBAL,
      GET_GLOBAL,
      SET_GLOBAL,
      GET_LOCAL,
      SET_LOCAL,
      JUMP_IF_FALSE,
      JUMP,
      LOOP
    };

    class Chunk {
    public:
      void write(const byte_code_t& code, int_t line);
      void replace(size_t offset, const byte_code_t& code);
      size_t addConstant(const value_t& value);
      size_t size(void) const;
      void disassemble(const string_t& name) const;

      const OpCode& readByte(size_t offset);
      size_t readOffset(size_t offset);
      const value_t& readConstant(size_t offset);
      void printValue(const value_t& value) const;
    private:
      size_t disassemble(size_t offset) const;

      size_t simpleInstruction(const string_t& name, size_t offset) const;
      size_t constantInstruction(const string_t& name, size_t offset) const;
      size_t byteInstruction(const string_t& name, size_t offset) const;
      size_t jumpInstruction(const string_t& name, int_t sign, size_t offset) const;
    private:
      byte_code_vec_t m_ByteCodes;
      value_vec_t m_Constants;
      int_vec_t m_Lines;
    };
  }
}
