#pragma once

#include "common.hpp"

namespace clox {
  namespace obj {
    class Object {
    public:
      Object(void) = default;
      virtual ~Object(void) = default;
    public:
      virtual void print(void) const = 0;
    public:
      static obj_ptr_t formStringObject(const string_t& str);
    };

    class String final : public Object {
    public:
      String(const string_t& str);
      virtual ~String(void) = default;
    public:
      const string_t& getString(void) const;
      virtual void print(void) const override;
    private:
      string_t m_Str;
    };

  }
}