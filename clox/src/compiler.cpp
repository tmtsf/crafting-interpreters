#pragma once

#include "compiler.hpp"
#include "scanner.hpp"
#include "chunk.hpp"
#include "object.hpp"

namespace clox {
  namespace compiler {
    Scope::Scope(const scope_ptr_t& enclosing,
                 const FunctionType& type) :
      m_Enclosing(enclosing),
      m_Function(nullptr),
      m_Type(type),
      m_Locals(),
      m_Depth(0)
    {
      m_Function = obj::Object::formFunctionObject();
      m_Locals.emplace_back(Token(TokenType(), "", 0), 0);
    }

    Compiler::Compiler(void) :
      m_Scope(nullptr, FunctionType::SCRIPT),
      m_Parser(),
      m_Scanner()
    {
      m_Chunk = &m_Scope.m_Function->m_Chunk;
    }

    function_ptr_t Compiler::compile(const string_t& source) {
      m_Scanner.set(source);
      advance();

      while (!match(TokenType::END_OF_FILE))
        declaration();

      function_ptr_t function = endCompiler();

      return m_Parser.m_HadError ? nullptr : function;
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
      } else if (match(TokenType::FUN)) {
        funDeclaration();
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

    void Compiler::funDeclaration(void) {
      size_t global = parseVariable("Expect function name.");
      markInitialized();
      function(FunctionType::FUNCTION);
      defineVariable(global);
    }

    void Compiler::statement(void) {
      if (match(TokenType::PRINT)) {
        printStatement();
      } else if (match(TokenType::LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
      } else if (match(TokenType::IF)) {
        ifStatement();
      } else if (match(TokenType::WHILE)) {
        whileStatement();
      } else if (match(TokenType::FOR)) {
        forStatement();
      } else if (match(TokenType::RETURN)) {
        returnStatement();
      } else {
        expressionStatement();
      }
    }

    void Compiler::printStatement(void) {
      expression();
      consume(TokenType::SEMICOLON, "Expect ';' after value.");
      emitByte(OpCode::PRINT);
    }

    void Compiler::block(void) {
      while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE))
        declaration();

      consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    }

    void Compiler::expressionStatement(void) {
      expression();
      consume(TokenType::SEMICOLON, "Expect ';' after expression.");
      emitByte(OpCode::POP);
    }

    void Compiler::ifStatement(void) {
      consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
      expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

      size_t thenJump = emitJump(OpCode::JUMP_IF_FALSE);
      emitByte(OpCode::POP);
      statement();

      size_t elseJump = emitJump(OpCode::JUMP);

      patchJump(thenJump);
      emitByte(OpCode::POP);

      if (match(TokenType::ELSE))
        statement();

      patchJump(elseJump);
    }

    void Compiler::whileStatement(void) {
      size_t start = m_Chunk->size();
      consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
      expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

      size_t exitJump = emitJump(OpCode::JUMP_IF_FALSE);
      emitByte(OpCode::POP);
      statement();
      emitLoop(start);

      patchJump(exitJump);
      emitByte(OpCode::POP);
    }

    void Compiler::forStatement(void) {
      beginScope();
      consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
      if (match(TokenType::SEMICOLON)) {
        ;                 // empty initializer, do nothing
      } else if (match(TokenType::VAR)) {
        varDeclaration();
      } else {
        expressionStatement();
      }

      size_t loopStart = m_Chunk->size();
      size_t exitJump = SIZE_MAX;
      if (!match(TokenType::SEMICOLON)) {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        exitJump = emitJump(OpCode::JUMP_IF_FALSE);
        emitByte(OpCode::POP);
      }

      if (!match(TokenType::RIGHT_PAREN)) {
        size_t bodyJump = emitJump(OpCode::JUMP);
        size_t incrStart = m_Chunk->size();
        expression();
        emitByte(OpCode::POP);
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrStart;
        patchJump(bodyJump);
      }

      statement();
      emitLoop(loopStart);

      if (exitJump != SIZE_MAX) {
        patchJump(exitJump);
        emitByte(OpCode::POP);
      }

      endScope();
    }

    void Compiler::returnStatement(void) {
      if (m_Scope.m_Type == FunctionType::SCRIPT)
        error("Cannot return from top-level code.");

      if (match(TokenType::SEMICOLON)) {
        emitReturn();
      } else {
        expression();
        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        emitByte(OpCode::RETURN);
      }
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

    void Compiler::beginScope(void) {
      ++m_Scope.m_Depth;
    }

    void Compiler::endScope(void) {
      --m_Scope.m_Depth;

      while (!m_Scope.m_Locals.empty() && m_Scope.m_Locals.back().m_Depth > m_Scope.m_Depth) {
        emitByte(OpCode::POP);
        m_Scope.m_Locals.pop_back();
      }
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
      emitByte(OpCode::NIL);
      emitByte(OpCode::RETURN);
    }

    size_t Compiler::emitJump(const OpCode& jump) {
      emitByte(jump);
      emitByte(SIZE_MAX);
      return m_Chunk->size() - 1;
    }

    void Compiler::patchJump(size_t offset) {
      size_t jump = m_Chunk->size() - offset - 1;
      m_Chunk->replace(offset, jump);
    }

    void Compiler::emitLoop(size_t start) {
      emitByte(OpCode::LOOP);

      size_t offset = m_Chunk->size() - start + 1;
      emitByte(offset);
    }

    function_ptr_t Compiler::endCompiler(void) {
      emitReturn();

      function_ptr_t function = m_Scope.m_Function;
      if (m_Scope.m_Enclosing) {
        m_Scope = *m_Scope.m_Enclosing;
        m_Chunk = &m_Scope.m_Function->m_Chunk;
      }

      return function;
    }

    size_t Compiler::makeConstant(const value_t& value) {
      size_t offset = m_Chunk->addConstant(value);
      return offset;
    }

    size_t Compiler::parseVariable(const char* message) {
      consume(TokenType::IDENTIFIER, message);

      declareVariable();
      if (m_Scope.m_Depth > 0)
        return 0;

      return identifierConstant(m_Parser.m_Prev);
    }

    void Compiler::defineVariable(size_t global) {
      if (m_Scope.m_Depth > 0) {
        markInitialized();
        return;
      }

      emitBytes({ OpCode::DEFINE_GLOBAL, global });
    }

    void Compiler::declareVariable(void) {
      if (m_Scope.m_Depth == 0)
        return;

      const Token& local = m_Parser.m_Prev;
      if (!m_Scope.m_Locals.empty()) {
        for (auto it = m_Scope.m_Locals.crbegin() + 1U; it != m_Scope.m_Locals.crend(); ++it) {
          if (it->m_Depth != -1 && it->m_Depth < m_Scope.m_Depth)
            break;

          if (local.m_Lexeme.compare(it->m_Token.m_Lexeme) == 0)
            error("Already a variable with this name in this scope");
        }
      }

      addLocal(local);
    }

    void Compiler::addLocal(const Token& token) {
      m_Scope.m_Locals.emplace_back(token, -1);
    }

    void Compiler::markInitialized(void) {
      if (m_Scope.m_Depth == 0)
        return;

      m_Scope.m_Locals.back().m_Depth = m_Scope.m_Depth;
    }

    size_t Compiler::identifierConstant(const Token& token) {
      return makeConstant(clox::obj::Object::formStringObject(token.m_Lexeme));
    }

    parse_rule_table_t Compiler::getParseRules(void) {
      static parse_rule_table_t rules = {
        {TokenType::LEFT_PAREN,       {&Compiler::grouping,   &Compiler::call,      Precedence::CALL}},
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
        {TokenType::AND,              {nullptr,               &Compiler::and_,      Precedence::AND}},
        {TokenType::CLASS,            {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::ELSE,             {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::FALSE,            {&Compiler::literal,    nullptr,              Precedence::NONE}},
        {TokenType::FOR,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::FUN,              {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::IF,               {nullptr,               nullptr,              Precedence::NONE}},
        {TokenType::NIL,              {&Compiler::literal,    nullptr,              Precedence::NONE}},
        {TokenType::OR,               {nullptr,               &Compiler::or_,       Precedence::OR}},
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

    void Compiler::and_(bool canAssign) {
      size_t endJump = emitJump(OpCode::JUMP_IF_FALSE);

      emitByte(OpCode::POP);
      parse(Precedence::AND);

      patchJump(endJump);
    }

    void Compiler::or_(bool canAssign) {
      size_t elseJump = emitJump(OpCode::JUMP_IF_FALSE);
      size_t endJump = emitJump(OpCode::JUMP);

      patchJump(elseJump);
      emitByte(OpCode::POP);

      parse(Precedence::OR);
      patchJump(endJump);
    }

    void Compiler::call(bool canAssign) {
      size_t count = argumentList();
      emitBytes({ OpCode::CALL, count });
    }

    void Compiler::namedVariable(const Token& token, bool canAssign) {
      int_t arg = resolveLocal(token);
      size_t offset;

      OpCode getOp;
      OpCode setOp;
      if (arg != -1) {
        offset = static_cast<size_t>(arg);
        getOp = OpCode::GET_LOCAL;
        setOp = OpCode::SET_LOCAL;
      } else {
        offset = identifierConstant(token);
        getOp = OpCode::GET_GLOBAL;
        setOp = OpCode::SET_GLOBAL;
      }

      if (canAssign && match(TokenType::EQUAL)) {
        expression();
        emitBytes({ setOp, offset });
      } else {
        emitBytes({ getOp, offset });
      }
    }

    int_t Compiler::resolveLocal(const Token& token) {      
      for (int_t i = static_cast<int_t>(m_Scope.m_Locals.size() - 1); i >= 0; --i) {
        const Local& local = m_Scope.m_Locals[i];
        if (token.m_Lexeme.compare(local.m_Token.m_Lexeme) == 0) {
          if (local.m_Depth == -1)
            error("Cannot read local variable in its own initializer.");

          return i;
        }
      }

      return -1;
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

    void Compiler::function(const FunctionType& type) {
      Scope scope = m_Scope;
      m_Scope = Scope(&scope, FunctionType::FUNCTION);
      m_Chunk = &m_Scope.m_Function->m_Chunk;
      m_Scope.m_Function->m_Name = m_Parser.m_Prev.m_Lexeme;
      beginScope();

      consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
      if (!check(TokenType::RIGHT_PAREN)) {
        do {
          m_Scope.m_Function->m_Arity++;
          size_t constant = parseVariable("Expect parameter name.");
          defineVariable(constant);
        } while (match(TokenType::COMMA));
      }
      consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
      consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
      block();

      function_ptr_t function = endCompiler();
      emitBytes({ OpCode::CLOSURE, makeConstant(function)});
    }

    size_t Compiler::argumentList(void) {
      size_t count = 0;
      if (!check(TokenType::RIGHT_PAREN)) {
        do {
          expression();
          count++;
        } while (match(TokenType::COMMA));
      }

      consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
      return count;
    }
  }
}