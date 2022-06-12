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
      static obj_ptr_t formStringObject(const string_t& str);
    };

    struct String final : public Object {
    public:
      String(const string_t& str);
      virtual ~String(void) = default;
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
  }
}