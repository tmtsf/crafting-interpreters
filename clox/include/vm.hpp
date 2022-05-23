#pragma once

#include "common.hpp"
#include "chunk.hpp"

namespace clox {

  namespace vm {
    enum class InterpretResult {
      OK,
      COMPILE_ERROR,
      RUNTIME_ERROR,
    };

    class VM {
    public:
      VM(void);
      ~VM(void) = default;

      // Do not allow copy or move the virtual machine
      VM(const VM&) = delete;
      VM(VM&&) = delete;
      VM& operator=(const VM&) = delete;
      VM& operator=(VM&&) = delete;

      InterpretResult interpret(void);
      InterpretResult interpret(const string_t& source);
    private:
      const OpCode& readByte(void);
      const size_t readOffset(void);
      const value_t& readConstant(void);
      const string_t& readString(void);

      void binaryAdd(void);
      void binaryOp(char c);
      InterpretResult run(void);
      const value_t& peek(size_t offset) const;

      bool isFalsey(const value_t& value) const;
      bool areEqual(const value_t& left, const value_t& right) const;
    private:
      Chunk m_Chunk;
      size_t m_IP; // Instruction pointer
      value_stack_t m_Stack;
      global_table_t m_Globals;
    };
  }
}