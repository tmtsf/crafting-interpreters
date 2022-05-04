#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <stack>

namespace clox {

  namespace vm {
    enum class OpCode;
    class Chunk;
    class VM;
  }

  using byte_code_t = std::variant<vm::OpCode, size_t>;
  using byte_code_vec_t = std::vector<byte_code_t>;

  using value_t = double;
  using value_vec_t = std::vector<value_t>;

  using int_vec_t = std::vector<int>;

  using chunk_ptr_t = std::shared_ptr<vm::Chunk>;

  using value_stack_t = std::stack<value_t>;
}