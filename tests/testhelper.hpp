#pragma once

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>


namespace jdl {
  namespace _details {

    template <typename SINGLETON_IMPL_TYPE>
    class singleton : SINGLETON_IMPL_TYPE {
      static singleton* get_internal() {
        static singleton* instance = nullptr;
        if (!instance) {
            instance = new singleton;
        }

        return instance;
      }

    public:
      static SINGLETON_IMPL_TYPE& get() {
        return *get_internal();
      }
    };

  } /* _details:: */


  class session {
    std::vector<std::function<void()>> cases_;
  public:
    void add(std::function<void()> f) {
      cases_.emplace_back(f);
    }

    void run() {
      for (const auto& f : cases_) {
        f();
      }
    }
  };


  using singl_session_t = _details::singleton<session>;

  struct registration {
    explicit registration(std::function<void()> f) {
      singl_session_t::get().add(f);
    }
  };

} /* jdl:: */


#define SUPPRESS_GLOBALS_WARNINGS                           \
  _Pragma("GCC diagnostic ignored \"-Wmissing-noreturn\"")  \
  _Pragma("GCC diagnostic ignored \"-Wsuggest-attribute=noreturn\"")


#define START_WARNINGS_SUPPRESSION _Pragma( "GCC diagnostic push" )
#define STOP_WARNINGS_SUPPRESSION  _Pragma( "GCC diagnostic pop" )

#define UNIQUE_NAME_LINE_MAIN(name, line) name##line
#define UNIQUE_NAME_LINE(name, line) UNIQUE_NAME_LINE_MAIN(name, line)

#define TEST_CASE_MY_MAIN( name, ... )                                  \
  START_WARNINGS_SUPPRESSION                                            \
  SUPPRESS_GLOBALS_WARNINGS                                             \
  static void name();                                                   \
  namespace{ jdl::registration UNIQUE_NAME_LINE(reg, __LINE__)(name); } \
  STOP_WARNINGS_SUPPRESSION                                             \
  void name()

#define TEST_CASE_MY( ... ) \
    TEST_CASE_MY_MAIN( UNIQUE_NAME_LINE(test_case_, __LINE__), __VA_ARGS__ )


// ASSERTIONS =====================================================================================
#define ASSERT_EQUAL(x, y)                                       \
{                                                                \
  if (( x ) != ( y )) {                                          \
    throw std::runtime_error(  std::string(__FILE__)             \
                             + std::string(":")                  \
                             + std::to_string(__LINE__)          \
                             + std::string(" in ")               \
                             + std::string(__PRETTY_FUNCTION__)  \
                             + std::string(": ")                 \
                             + std::to_string(( x ))             \
                             + std::string(" != ")               \
                             + std::to_string(( y ))             \
    );                                                           \
  }                                                              \
}

#define ASSERT_STREQUAL(x, y)                                    \
{                                                                \
  if (( x ).compare(( y )) != 0) {                               \
    throw std::runtime_error(  std::string(__FILE__)             \
                             + std::string(":")                  \
                             + std::to_string(__LINE__)          \
                             + std::string(" in ")               \
                             + std::string(__PRETTY_FUNCTION__)  \
                             + std::string(": ")                 \
                             + std::string(( x ))                \
                             + std::string(" != ")               \
                             + std::string(( y ))                \
    );                                                           \
  }                                                              \
}

#define ASSERT_THROW(condition)                                  \
{                                                                \
  if (!(condition)) {                                            \
    throw std::runtime_error(  std::string(__FILE__)             \
                             + std::string(":")                  \
                             + std::to_string(__LINE__)          \
                             + std::string(" in ")               \
                             + std::string(__PRETTY_FUNCTION__)  \
    );                                                           \
  }                                                              \
}


#define EXPECT_EXCEPTION(expression, exception)                  \
{                                                                \
  try {                                                          \
    (expression);                                                \
  }                                                              \
  catch(exception& e) { }                                        \
  catch( ... ) {                                                 \
    throw std::runtime_error(  std::string(__FILE__)             \
                             + std::string(":")                  \
                             + std::to_string(__LINE__)          \
                             + std::string(" in ")               \
                             + std::string(__PRETTY_FUNCTION__)  \
    );                                                           \
  }                                                              \
}












int main(/*int argc, char *argv[]*/) {
  jdl::singl_session_t::get().run();
}
