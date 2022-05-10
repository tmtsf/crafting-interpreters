#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>
#include <functional>

namespace clox {

  namespace vm {
    enum class OpCode;
    class Chunk;

    enum class InterpretResult;
    class VM;
  }

  namespace scanner {
    enum class TokenType;
    struct Token;
    class Scanner;
  }

  namespace compiler {
    struct Parser;
    class Compiler;
  }

  using dbl_t = double;
  using int_t = int;
  using string_t = std::string;

  using byte_code_t = std::variant<vm::OpCode, size_t>;
  using byte_code_vec_t = std::vector<byte_code_t>;

  using value_t = dbl_t;
  using value_vec_t = std::vector<value_t>;

  using int_vec_t = std::vector<int_t>;

  using chunk_ptr_t = std::shared_ptr<vm::Chunk>;

  using value_stack_t = std::vector<value_t>;
}