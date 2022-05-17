#pragma once

#include "compiler.hpp"
#include "scanner.hpp"
#include "chunk.hpp"
#include "object.hpp"

namespace clox {
  namespace compiler {
    //void Compiler::compile(const string_t& source) {
    //  Scanner scanner(source);

    //  int line = -1;
    //  for (; ; ) {
    //    Token token = scanner.scan();
    //    if (token.m_Line != line) {
    //      printf("%4d ", token.m_Line);
    //      line = token.m_Line;
    //    }
    //    else {
    //      printf("   | ");
    //    }

    //    printf("%2d '%s'\n", token.m_Type, token.m_Lexeme.c_str());

    //    if (token.m_Type == TokenType::END_OF_FILE)
    //      break;
    //  }
    //}

    Compiler::Compiler(const chunk_ptr_t& chunk) :
      m_Chunk(chunk),
      m_Parser(),
      m_Scanner()
    { }

    bool Compiler::compile(const string_t& source) {
      m_Scanner.set(source);
      advance();

      while (!match(TokenType::END_OF_FILE))
        declaration();

      endCompiler();

      return !m_Parser.m_HadError;
    }

    void Compiler::advance(void) {
      m_Parser.m_Prev = m_Parser.m_Curr;

      while (true) {
        m_Parser.m_Curr = m_Scanner.scan();
        if (m_Parser.m_Curr.m_Type != TokenType::ERROR)
          break;

        errorAtCurrent(m_Parser.m_Curr.m_Lexeme.c_str());
      }
    }

    void Compiler::expression(void) {
      parse(Precedence::ASSIGNMENT);
    }

    void Compiler::declaration(void) {
      if (match(TokenType::VAR)) {
        varDeclaration();
      } else {
        statement();
      }
    }

    void Compiler::varDeclaration(void) {
      size_t global = parseVariable("Expect variable name.");

      if (match(TokenType::EQUAL)) {
        expression();
      } else {
        emitByte(OpCode::NIL);
      }

      consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

      defineVariable(global);
    }

    void Compiler::statement(void) {
      if (match(TokenType::PRINT)) {
        printStatement();
      } else {
        expressionStatement();
      }
    }

    void Compiler::printStatement(void) {
      expression();
      consume(TokenType::SEMICOLON, "Expect ';' after value.");
      emitByte(OpCode::PRINT);
    }

    void Compiler::expressionStatement(void) {
      expression();
      consume(TokenType::SEMICOLON, "Expect ';' after expression.");
      emitByte(OpCode::POP);
    }

    void Compiler::consume(const TokenType& type, const string_t& message) {
      if (m_Parser.m_Curr.m_Type == type) {
        advance();
        return;
      }

      errorAtCurrent(message.c_str());
    }

    void Compiler::errorAtCurrent(const char* message) {
      errorAt(m_Parser.m_Curr, message);
    }

    void Compiler::error(const char* message) {
      errorAt(m_Parser.m_Prev, message);
    }

    void Compiler::errorAt(const Token& token, const char* message) {
      if (m_Parser.m_Panic)
        return;

      m_Parser.m_Panic = true;
      fprintf(stderr, "[line %d] Error", token.m_Line);

      if (token.m_Type == TokenType::END_OF_FILE)
        fprintf(stderr, " at end");
      else if (token.m_Type == TokenType::ERROR)
        ; //
      else
        fprintf(stderr, " at '%s'", token.m_Lexeme.c_str());

      fprintf(stderr, ": %s\n", message);
      m_Parser.m_HadError = true;
    }

    bool Compiler::check(const TokenType& type) const {
      return m_Parser.m_Curr.m_Type == type;
    }

    bool Compiler::match(const TokenType& type) {
      if (!check(type))
        return false;

      advance();
      return true;
    }

    void Compiler::emitByte(const byte_code_t& code) {
      m_Chunk->write(code, m_Parser.m_Prev.m_Line);
    }

    void Compiler::emitBytes(const byte_code_vec_t& codes) {
      for (const auto& code : codes)
        emitByte(code);
    }

    void Compiler::emitConstant(const value_t& value) {
      emitBytes({ OpCode::CONSTANT, makeConstant(value) });
    }

    void Compiler::emitReturn(void) {
      emitByte(OpCode::RETURN);
    }

    void Compiler::endCompiler(void) {
      emitReturn();
    }

    size_t Compiler::makeConstant(const value_t& value) {
      size_t offset = m_Chunk->addConstant(value);
      return offset;
    }

    size_t Compiler::parseVariable(const char* message) {
      consume(TokenType::IDENTIFIER, message);
      return identifierConstant(m_Parser.m_Prev);
    }

    void Compiler::defineVariable(size_t global) {
      emitBytes({ OpCode::DEFINE_GLOBAL, global });
    }

    size_t Compiler::identifierConstant(const Token& token) {
      return makeConstant(clox::obj::Object::formStringObject(token.m_Lexeme));
    }

    parse_rule_table_t Compiler::getParseRules(void) {
      static parse_rule_table_t rules = {
        {TokenType::LEFT_PAREN,       {&Compiler::grouping,   nullptr,              Precedence::NONE}},
        {TokenType::RIGHT_PAREN,      {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::LEFT_BRACE,       {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::RIGHT_BRACE,      {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::COMMA,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::DOT,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::MINUS,            {&Compiler::unary,      &Compiler::binary,    Precedence::TERM}},
        {TokenType::PLUS,             {&Compiler::unary,      &Compiler::binary,    Precedence::TERM}},
        {TokenType::SEMICOLON,        {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::SLASH,            {nullptr,               &Compiler::binary,    Precedence::FACTOR}},
        {TokenType::STAR,             {nullptr,               &Compiler::binary,    Precedence::FACTOR}},
        {TokenType::BANG,             {&Compiler::unary,      nullptr,              Precedence::NONE}},
        {TokenType::BANG_EQUAL,       {nullptr,               &Compiler::binary,    Precedence::EQUALITY}},
        {TokenType::EQUAL,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::EQUAL_EQUAL,      {nullptr,               &Compiler::binary,    Precedence::EQUALITY}},
        {TokenType::GREATER,          {nullptr,               &Compiler::binary,    Precedence::COMPARISON}},
        {TokenType::GREATER_EQUAL,    {nullptr,               &Compiler::binary,    Precedence::COMPARISON}},
        {TokenType::LESS,             {nullptr,               &Compiler::binary,    Precedence::COMPARISON}},
        {TokenType::LESS_EQUAL,       {nullptr,               &Compiler::binary,    Precedence::COMPARISON}},
        {TokenType::IDENTIFIER,       {&Compiler::variable,   nullptr,              Precedence::NONE}},
        {TokenType::STRING,           {&Compiler::string,     nullptr,              Precedence::NONE}},
        {TokenType::NUMBER,           {&Compiler::number,     nullptr,              Precedence::NONE}},
        {TokenType::AND,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::CLASS,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::ELSE,             {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::FALSE,            {&Compiler::literal,    nullptr,              Precedence::NONE}},
        {TokenType::FOR,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::FUN,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::IF,               {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::NIL,              {&Compiler::literal,    nullptr,              Precedence::NONE}},
        {TokenType::OR,               {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::PRINT,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::RETURN,           {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::SUPER,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::THIS,             {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::TRUE,             {&Compiler::literal,    nullptr,              Precedence::NONE}},
        {TokenType::VAR,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::WHILE,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::ERROR,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::END_OF_FILE,      {nullptr,               nullptr,              Precedence::NONE}}
      };

      return rules;
    }

    void Compiler::number(bool canAssign) {
      dbl_t value = std::stod(m_Parser.m_Prev.m_Lexeme);
      emitConstant(value);
    }

    void Compiler::grouping(bool canAssign) {
      expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    }

    void Compiler::unary(bool canAssign) {
      TokenType type = m_Parser.m_Prev.m_Type;

      parse(Precedence::UNARY);

      switch (type) {
      case TokenType::MINUS:
        emitByte(OpCode::NEGATE);
        break;
      case TokenType::BANG:
        emitByte(OpCode::NOT);
        break;
      case TokenType::PLUS:
      default:
        return;
      }
    }

    void Compiler::binary(bool canAssign) {
      TokenType type = m_Parser.m_Prev.m_Type;
      const ParseRule& rule = parseRule(type);
      parse(static_cast<Precedence>(static_cast<int>(rule.m_Prec) + 1));

      switch (type) {
      case TokenType::PLUS:
        emitByte(OpCode::ADD);
        break;
      case TokenType::MINUS:
        emitByte(OpCode::SUBTRACT);
        break;
      case TokenType::STAR:
        emitByte(OpCode::MULTIPLY);
        break;
      case TokenType::SLASH:
        emitByte(OpCode::DIVIDE);
        break;
      case TokenType::BANG_EQUAL:
        emitBytes({ OpCode::EQUAL, OpCode::NOT });
        break;
      case TokenType::EQUAL_EQUAL:
        emitByte(OpCode::EQUAL);
        break;
      case TokenType::GREATER:
        emitByte(OpCode::GREATER);
        break;
      case TokenType::GREATER_EQUAL:
        emitBytes({ OpCode::LESS, OpCode::NOT });
        break;
      case TokenType::LESS:
        emitByte(OpCode::LESS);
        break;
      case TokenType::LESS_EQUAL:
        emitBytes({ OpCode::GREATER, OpCode::NOT });
        break;
      default:
        return;
      }
    }

    void Compiler::literal(bool canAssign) {
      switch (m_Parser.m_Prev.m_Type) {
      case TokenType::FALSE:
        emitByte(OpCode::FALSE);
        break;
      case TokenType::NIL:
        emitByte(OpCode::NIL);
        break;
      case TokenType::TRUE:
        emitByte(OpCode::TRUE);
        break;
      default:
        break;
      }
    }

    void Compiler::string(bool canAssign) {
      emitConstant(clox::obj::Object::formStringObject(
                        string_t(m_Parser.m_Prev.m_Lexeme.cbegin() + 1U,
                                 m_Parser.m_Prev.m_Lexeme.cend() - 1U)));
    }

    void Compiler::variable(bool canAssign) {
      namedVariable(m_Parser.m_Prev, canAssign);
    }

    void Compiler::namedVariable(const Token& token, bool canAssign) {
      size_t offset = identifierConstant(token);

      if (canAssign && match(TokenType::EQUAL)) {
        expression();
        emitBytes({ OpCode::SET_GLOBAL, offset });
      } else {
        emitBytes({ OpCode::GET_GLOBAL, offset });
      }
    }

    ParseRule Compiler::parseRule(const TokenType& type) {
      return getParseRules()[type];
    }

    void Compiler::parse(const Precedence& prec) {
      advance();
      parse_func_t prefix = parseRule(m_Parser.m_Prev.m_Type).m_Prefix;
      if (!prefix) {
        error("Expect expression.");
        return;
      }

      bool canAssign = prec <= Precedence::ASSIGNMENT;
      (this->*prefix)(canAssign);

      while (prec <= parseRule(m_Parser.m_Curr.m_Type).m_Prec) {
        advance();
        parse_func_t infix = parseRule(m_Parser.m_Prev.m_Type).m_Infix;
        (this->*infix)(canAssign);
      }

      if (canAssign && match(TokenType::EQUAL))
        error("Invalid assignment target.");
    }
  }
}