#pragma once

#include "common.hpp"

namespace clox {
  namespace scanner {

    enum class TokenType {
      // Single-character tokens.
      TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
      TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
      TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
      TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
      // One or two character tokens.
      TOKEN_BANG, TOKEN_BANG_EQUAL,
      TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
      TOKEN_GREATER, TOKEN_GREATER_EQUAL,
      TOKEN_LESS, TOKEN_LESS_EQUAL,
      // Literals.
      TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
      // Keywords.
      TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
      TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
      TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
      TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

      TOKEN_ERROR, TOKEN_EOF
    };

    struct Token {
      TokenType m_Type;
      int m_Start;
      int m_Length;
      int m_Line;
    };

    class Scanner {
    public:
      Scanner(const std::string& source);
      Token scan();

    private:
      bool isAtEnd(void) const;
      Token makeToken(const TokenType&) const;
      Token errorToken(const std::string& message) const;
      char advance(void);
      bool match(char expected);
      void skipWhitespace(void);
      char peek(void) const;
      char peekNext(void) const;
      Token string(void);
      bool isDigit(char c) const;
      Token number(void);
      bool isAlpha(char c) const;
      Token identifier(void);
      TokenType identifierType(void) const;

    private:
      const std::string& m_Source;
      int m_Start;
      int m_Current;
      int m_Line;
    };
  }
}