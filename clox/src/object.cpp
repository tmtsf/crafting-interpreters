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

    void String::print(void) const {
      printf("%s", m_Str.c_str());
    }

    Function::Function(void) :
      m_Name(""),
      m_Arity(0),
      m_Chunk()
    { }

    void Function::print(void) const {
      if (m_Name.empty()) {
        printf("<script>");
        return;
      }

      printf("<fn %s>", m_Name.c_str());
    }
  }
}