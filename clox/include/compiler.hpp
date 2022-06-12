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

    using parse_func_t = void (Compiler::*)(bool);

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

    struct Local {
      Token m_Token;
      int m_Depth;
    };

    enum class FunctionType {
      FUNCTION,
      SCRIPT
    };

    struct Scope {
      Scope(const function_ptr_t& function,
            const FunctionType& type) :
        m_Function(function),
        m_Type(type),
        m_Locals(),
        m_Depth(0)
      {
        m_Locals.emplace_back(Token(TokenType(), "", 0), 0);
      }

      function_ptr_t m_Function;
      FunctionType m_Type;

      local_vec_t m_Locals;
      int_t m_Depth;
    };

    using parse_rule_table_t = std::unordered_map<TokenType, ParseRule, Hash>;

    class Compiler {
    public:
      //void compile(const string_t& source);
      Compiler(Chunk& chunk);
      function_ptr_t compile(const string_t& source);
    private:
      void advance(void);
      void consume(const TokenType& type, const string_t& message);

      void declaration(void);

      void varDeclaration(void);

      void statement(void);
      void printStatement(void);
      void block(void);
      void expressionStatement(void);
      void ifStatement(void);
      void whileStatement(void);
      void forStatement(void);

      void expression(void);

      void errorAtCurrent(const char* message);
      void error(const char* message);
      void errorAt(const Token& token, const char* message);

      bool match(const TokenType& type);
      bool check(const TokenType& type) const;

      void beginScope(void);
      void endScope(void);

    private:
      void emitByte(const byte_code_t& code);
      void emitBytes(const byte_code_vec_t& codes);
      void emitConstant(const value_t& value);
      void emitReturn(void);
      size_t emitJump(const OpCode& jump);
      void patchJump(size_t offset);
      void emitLoop(size_t start);
      function_ptr_t endCompiler(void);

      size_t makeConstant(const value_t& value);
      size_t parseVariable(const char* message);
      size_t identifierConstant(const Token& token);
      void defineVariable(size_t offset);
      void declareVariable(void);
      void addLocal(const Token& token);
      void markInitialized(void);

      void number(bool canAssign);
      void grouping(bool canAssign);
      void unary(bool canAssign);
      void binary(bool canAssign);
      void literal(bool canAssign);
      void string(bool canAssign);
      void variable(bool canAssign);
      void and_(bool canAssign);
      void or_(bool canAssign);

      void parse(const Precedence& prec);
      void namedVariable(const Token& token, bool canAssign);

      int_t resolveLocal(const Token& token);

      static parse_rule_table_t getParseRules(void);
      ParseRule parseRule(const TokenType& type);
    private:
      Chunk& m_Chunk;
      Scope m_Scope;
      Parser m_Parser;
      Scanner m_Scanner;
    };
  }
}