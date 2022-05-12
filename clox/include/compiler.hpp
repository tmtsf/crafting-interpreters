#pragma once

#include "common.hpp"
#include "scanner.hpp"

namespace clox {
  using namespace scanner;
  using namespace vm;

  namespace compiler {

    struct Parser {
      Parser(void) :
        m_Prev(),
        m_Curr(),
        m_HadError(false),
        m_Panic(false)
      { }

      Token m_Prev;
      Token m_Curr;
      bool m_HadError;
      bool m_Panic;
    };

    enum class Precedence {
      NONE,
      ASSIGNMENT,
      OR,
      AND,
      EQUALITY,
      COMPARISON,
      TERM,
      FACTOR,
      UNARY,
      CALL,
      PRIMARY,
    };

    using parse_func_t = void (Compiler::*)(void);

    struct ParseRule {
      parse_func_t m_Prefix;
      parse_func_t m_Infix;
      Precedence m_Prec;
    };

    namespace {
      struct Hash {
        template<typename T>
        size_t operator()(const T& t) const {
          return static_cast<size_t>(t);
        }
      };
    }

    using parse_rule_table_t = std::unordered_map<TokenType, ParseRule, Hash>;

    class Compiler {
    public:
      //void compile(const string_t& source);
      Compiler(const chunk_ptr_t& chunk);
      bool compile(const string_t& source);
    private:
      void advance(void);
      void expression(void);
      void consume(const TokenType& type, const string_t& message);

      void errorAtCurrent(const char* message);
      void error(const char* message);
      void errorAt(const Token& token, const char* message);

    private:
      void emitByte(const byte_code_t& code);
      void emitBytes(const byte_code_vec_t& codes);
      void emitConstant(const value_t& value);
      void emitReturn(void);
      void endCompiler(void);

      size_t makeConstant(const value_t& value);

      void number(void);
      void grouping(void);
      void unary(void);
      void binary(void);
      void literal(void);
      void string(void);

      void parse(const Precedence& prec);

      static parse_rule_table_t getParseRules(void);
      ParseRule parseRule(const TokenType& type);
    private:
      chunk_ptr_t m_Chunk;
      Parser m_Parser;
      Scanner m_Scanner;
    };
  }
}