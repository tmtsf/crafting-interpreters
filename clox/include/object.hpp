#pragma once

#include "common.hpp"
#include "chunk.hpp"

namespace clox {
  namespace obj {
    struct Object {
    public:
      Object(void) = default;
      virtual ~Object(void) = default;
    public:
      virtual void print(void) const = 0;
    public:
      static string_ptr_t formStringObject(const string_t& str);
      static function_ptr_t formFunctionObject(void);
      static closure_ptr_t formClosureObject(const function_ptr_t& function);
    };

    struct String final : public Object {
    public:
      String(const string_t& str);
    public:
      virtual void print(void) const override;
    public:
      string_t m_Str;
    };

    struct Function final : public Object {
    public:
      Function(void);
    public:
      virtual void print(void) const override;
    public:
      string_t m_Name;
      size_t m_Arity;
      vm::Chunk m_Chunk;
    };

    struct Closure final : public Object {
    public:
      Closure(const function_ptr_t& function);
    public:
      virtual void print(void) const override;
    public:
      function_ptr_t m_Function;
    };
  }
}