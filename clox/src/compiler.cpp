#pragma once

#include "compiler.hpp"
#include "scanner.hpp"

namespace clox {
  namespace compiler {
    void Compiler::compile(const std::string& source) {
      clox::scanner::Scanner scanner(source);

      int line = -1;
      for (; ; ) {
        clox::scanner::Token token = scanner.scan();
        if (token.m_Line != line) {
          printf("%4d ", token.m_Line);
          line = token.m_Line;
        }
        else {
          printf("   | ");
        }

        printf("%2d '%s'\n", token.m_Type, source.substr(token.m_Start, token.m_Length).c_str());

        if (token.m_Type == clox::scanner::TokenType::TOKEN_EOF)
          break;
      }
    }
  }
}