#include "testhelper.hpp"

#include <jdl/httpclientlite.hpp>


TEST_CASE_MY("Test of tokenizer.next", "[tokenizer]") {
  using namespace jdl::http_client;

  std::string actual = method2string<http_method::GET>();
  ASSERT_STREQUAL(actual, std::string("GET"));
}
