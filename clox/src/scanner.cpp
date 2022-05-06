#pragma once

#include <scanner.hpp>

namespace clox {
  namespace scanner {

    Scanner::Scanner(const std::string& source) :
      m_Source(source),
      m_Start(0),
      m_Current(0),
      m_Line(1)
    { }

    Token Scanner::scan() {
      skipWhitespace();
      m_Start = m_Current;

      if (isAtEnd())
        return makeToken(TokenType::TOKEN_EOF);

      char c = advance();
      if (isDigit(c))
        return number();
      if (isAlpha(c))
        return identifier();

      switch (c)
      {
      case '(':
        return makeToken(TokenType::TOKEN_LEFT_PAREN);
      case ')':
        return makeToken(TokenType::TOKEN_RIGHT_PAREN);
      case '{':
        return makeToken(TokenType::TOKEN_LEFT_BRACE);
      case '}':
        return makeToken(TokenType::TOKEN_RIGHT_BRACE);
      case ';':
        return makeToken(TokenType::TOKEN_SEMICOLON);
      case ',':
        return makeToken(TokenType::TOKEN_COMMA);
      case '.':
        return makeToken(TokenType::TOKEN_DOT);
      case '+':
        return makeToken(TokenType::TOKEN_PLUS);
      case '-':
        return makeToken(TokenType::TOKEN_MINUS);
      case '*':
        return makeToken(TokenType::TOKEN_STAR);
      case '/':
        return makeToken(TokenType::TOKEN_SLASH);
      case '!':
        return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
      case '=':
        return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
      case '<':
        return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
      case '>':
        return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
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
      return Token(type, m_Start, m_Current - m_Start, m_Line);
    }

    Token Scanner::errorToken(const std::string& message) const {
      return Token(TokenType::TOKEN_ERROR, m_Start, m_Current - m_Start, m_Line);
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
      return makeToken(TokenType::TOKEN_STRING);
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

      return makeToken(TokenType::TOKEN_NUMBER);
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
      std::unordered_map<std::string, TokenType> m =
      {
        {"and",    TokenType::TOKEN_AND},
        {"class",  TokenType::TOKEN_CLASS},
        {"else",   TokenType::TOKEN_ELSE},
        {"if",     TokenType::TOKEN_IF},
        {"nil",    TokenType::TOKEN_NIL},
        {"or",     TokenType::TOKEN_OR},
        {"print",  TokenType::TOKEN_PRINT},
        {"return", TokenType::TOKEN_RETURN},
        {"super",  TokenType::TOKEN_SUPER},
        {"var",    TokenType::TOKEN_VAR},
        {"while",  TokenType::TOKEN_WHILE},
        {"false",  TokenType::TOKEN_FALSE},
        {"for",    TokenType::TOKEN_FOR},
        {"fun",    TokenType::TOKEN_FUN},
        {"this",   TokenType::TOKEN_THIS},
        {"true",   TokenType::TOKEN_TRUE}
      };
    }

    TokenType Scanner::identifierType(void) const {
      std::string lexeme = m_Source.substr(m_Start, m_Current - m_Start);
      if (m.count(lexeme))
        return m[lexeme];

      return TokenType::TOKEN_IDENTIFIER;
    }
  }
}