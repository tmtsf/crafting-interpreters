#include "object.hpp"

namespace clox {
  namespace obj {

    obj_ptr_t Object::formStringObject(const string_t& str) {
      return new String(str);
    }

    String::String(const string_t& str) :
      Object(),
      m_Str(str)
    { }

    const string_t& String::getString(void) const {
      return m_Str;
    }

    void String::print(void) const {
      printf("%s", m_Str.c_str());
    }
  }
}