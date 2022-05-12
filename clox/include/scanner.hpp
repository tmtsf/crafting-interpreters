#pragma once

#include "common.hpp"

namespace clox {
  namespace scanner {

    enum class TokenType {
      // Single-character tokens.
      LEFT_PAREN, 
      RIGHT_PAREN,
      LEFT_BRACE, 
      RIGHT_BRACE,
      COMMA, 
      DOT, 
      MINUS, 
      PLUS,
      SEMICOLON, 
      SLASH, 
      STAR,

      // One or two character tokens.
      BANG, 
      BANG_EQUAL,
      EQUAL, 
      EQUAL_EQUAL,
      GREATER, 
      GREATER_EQUAL,
      LESS, 
      LESS_EQUAL,

      // Literals.
      IDENTIFIER, 
      STRING, 
      NUMBER,

      // Keywords.
      AND, 
      CLASS, 
      ELSE, 
      FALSE,
      FOR, 
      FUN, 
      IF, 
      NIL, 
      OR,
      PRINT, 
      RETURN, 
      SUPER, 
      THIS,
      TRUE, 
      VAR, 
      WHILE,

      ERROR, 
      END_OF_FILE,
    };

    struct Token {
      TokenType m_Type;
      string_t m_Lexeme;
      int_t m_Line;
    };

    class Scanner {
    public:
      Scanner(void) = default;
      Token scan();
      void set(const string_t& source);

    private:
      bool isAtEnd(void) const;
      Token makeToken(const TokenType&) const;
      Token errorToken(const string_t& message) const;
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
      string_t m_Source;
      int_t m_Start;
      int_t m_Current;
      int_t m_Line;
    };
  }
}