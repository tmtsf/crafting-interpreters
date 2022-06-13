#include "object.hpp"

namespace clox {
  namespace obj {

    string_ptr_t Object::formStringObject(const string_t& str) {
      return new String(str);
    }

    function_ptr_t Object::formFunctionObject(void) {
      return new Function();
    }

    closure_ptr_t Object::formClosureObject(const function_ptr_t& function)
    {
      return new Closure(function);
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

    Closure::Closure(const function_ptr_t& function) :
      m_Function(function)
    { }

    void Closure::print(void) const {
      m_Function->print();
    }
  }
}