#pragma once

#include "scanner.hpp"

namespace clox {
  namespace scanner {

    Scanner::Scanner(const string_t& source) :
      m_Source(source),
      m_Start(0),
      m_Current(0),
      m_Line(1)
    { }

    Token Scanner::scan() {
      skipWhitespace();
      m_Start = m_Current;

      if (isAtEnd())
        return makeToken(TokenType::END_OF_FILE);

      char c = advance();
      if (isDigit(c))
        return number();
      if (isAlpha(c))
        return identifier();

      switch (c)
      {
      case '(':
        return makeToken(TokenType::LEFT_PAREN);
      case ')':
        return makeToken(TokenType::RIGHT_PAREN);
      case '{':
        return makeToken(TokenType::LEFT_BRACE);
      case '}':
        return makeToken(TokenType::RIGHT_BRACE);
      case ';':
        return makeToken(TokenType::SEMICOLON);
      case ',':
        return makeToken(TokenType::COMMA);
      case '.':
        return makeToken(TokenType::DOT);
      case '+':
        return makeToken(TokenType::PLUS);
      case '-':
        return makeToken(TokenType::MINUS);
      case '*':
        return makeToken(TokenType::STAR);
      case '/':
        return makeToken(TokenType::SLASH);
      case '!':
        return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      case '=':
        return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      case '<':
        return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      case '>':
        return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      case '"':
        return string();
      default:
        break;
      }

      return errorToken("Unexpected character.");
    }


    bool Scanner::isAtEnd(void) const {
      return m_Current == m_Source.size();
    }

    Token Scanner::makeToken(const TokenType& type) const {
      return Token(type, m_Source.substr(m_Start, m_Current - m_Start), m_Line);
    }

    Token Scanner::errorToken(const string_t& message) const {
      return Token(TokenType::ERROR, message, m_Line);
    }

    char Scanner::advance(void) {
      return m_Source[m_Current++];
    }

    bool Scanner::match(char expected) {
      if (isAtEnd())
        return false;

      if (m_Source[m_Current] != expected)
        return false;

      ++m_Current;
      return true;
    }

    void Scanner::skipWhitespace(void) {
      for ( ; ; ) {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
          advance();
          break;
        case '\n':
          ++m_Line;
          advance();
          break;
        case '/':
          if (peekNext() == '/') {
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd())
              advance();
          }
          else {
            return;
          }
          break;
        default:
          return;
        }
      }
    }

    char Scanner::peek(void) const {
      return m_Source[m_Current];
    }

    char Scanner::peekNext(void) const {
      if (m_Current < m_Source.size() - 1)
        return '\0';

      return m_Source[m_Current + 1];
    }

    Token Scanner::string(void) {
      while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n')
          ++m_Line;
        advance();
      }

      if (isAtEnd())
        return errorToken("Unterminated string.");

      advance();
      return makeToken(TokenType::STRING);
    }

    bool Scanner::isDigit(char c) const {
      return c >= '0' && c <= '9';
    }

    Token Scanner::number(void) {
      while (isDigit(peek()))
        advance();

      if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek()))
          advance();
      }

      return makeToken(TokenType::NUMBER);
    }

    bool Scanner::isAlpha(char c) const {
      return (c >= 'a' && c <= 'z') ||
             (c >= 'A' && c <= 'Z') ||
             c == '_';
    }

    Token Scanner::identifier(void) {
      while (isDigit(peek()) || isAlpha(peek()))
        advance();

      return makeToken(identifierType());
    }

    namespace {
      std::unordered_map<string_t, TokenType> m =
      {
        {"and",    TokenType::AND},
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"if",     TokenType::IF},
        {"nil",    TokenType::NIL},
        {"or",     TokenType::OR},
        {"print",  TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super",  TokenType::SUPER},
        {"var",    TokenType::VAR},
        {"while",  TokenType::WHILE},
        {"false",  TokenType::FALSE},
        {"for",    TokenType::FOR},
        {"fun",    TokenType::FUN},
        {"this",   TokenType::THIS},
        {"true",   TokenType::TRUE}
      };
    }

    TokenType Scanner::identifierType(void) const {
      string_t lexeme = m_Source.substr(m_Start, m_Current - m_Start);
      if (m.count(lexeme))
        return m[lexeme];

      return TokenType::IDENTIFIER;
    }
  }
}