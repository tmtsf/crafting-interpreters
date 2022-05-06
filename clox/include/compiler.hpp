#pragma once

#include "common.hpp"

namespace clox {
  namespace compiler {

    class Compiler {
    public:
      static void compile(const std::string& source);
    };
  }
}