#include "testhelper.hpp"

#include <jdl/httpclientlite.hpp>


TEST_CASE_MY("Test of tokenizer.next", "[tokenizer]") {
  jdl::tokenizer tz("My first test");

  std::string actual = tz.next("first");
  ASSERT_STREQUAL(actual, "My ");

  std::string actual2 = tz.next("first");
  ASSERT_STREQUAL(actual2, std::string(""));

  std::string actual3 = tz.next("first", true);
  ASSERT_STREQUAL(actual3, " test");

  std::string actual4 = tz.next("second");
  ASSERT_STREQUAL(actual4, std::string(""));
}


TEST_CASE_MY("Test of tokenizer.tail", "[tokenizer]") {
  jdl::tokenizer tz("My second test");

  std::string actual = tz.tail();
  ASSERT_STREQUAL(actual, "My second test");
}
