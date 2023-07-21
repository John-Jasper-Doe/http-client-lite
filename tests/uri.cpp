#include <jdl/httpclientlite.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace jdl::details_http_client_lite;

TEST_CASE("Test JDL of URI of base class", "[uri_base]") {
  std::string test_buff("foo://username:password@www.example.com:8080/hello/index.html");
  uri_base uri_base_test(test_buff);
  buff_offset data;

  // Check function get_token with end delimiter
  SECTION("- with end delimiter [uri_base.get_token]") {
    // With set parameters
    data = uri_base_test.get_token<0, 10, checker_default_t>("://");
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string("foo"));

    // With default parameters
    data = uri_base_test.get_token<>("://");
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string("foo"));

    // With default parameters and without delimiter
    data = uri_base_test.get_token<>("");
    CHECK(data == buff_offset());
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string());

    // Check on invalid arguments [POSITION]
    CHECK_THROWS_AS((uri_base_test.get_token<1000, 10, checker_default_t>("://")),
                    std::invalid_argument);

    CHECK_THROWS_WITH((uri_base_test.get_token<1000, 10, checker_default_t>("://")),
                      "buff_ref_.size() <= POSITION");

    // Check on invalid arguments [COUNT]
    CHECK_THROWS_AS((uri_base_test.get_token<1, 1000, checker_default_t>("://")),
                    std::invalid_argument);

    CHECK_THROWS_WITH((uri_base_test.get_token<1, 1000, checker_default_t>("://")),
                      "buff_ref_.size() < POSITION + COUNT");
  }

  // Check function get_token with start and end delimiter
  SECTION("- with start and end delimiter [uri_base.get_token]") {
    // With set parameters
    data = uri_base_test.get_token<0, 20, checker_default_t>("://", ":p");
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string("username"));

    // With set parameters and fragment not contain set delimiters
    data = uri_base_test.get_token<20, 20, checker_default_t>("://", ":p");
    CHECK(data == buff_offset());
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string());

    // With default parameters
    data = uri_base_test.get_token<>("://", ":p");
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string("username"));

    // With default parameters and without delimiters
    data = uri_base_test.get_token<>("", "");
    CHECK(data == buff_offset());
    CHECK(data.get(uri_base_test.buff_ref()).str() == std::string());
  }
}

TEST_CASE("Test JDL of URI of schema class", "[uri::schema]") {
  SECTION("- case #1 - Normal URI [schema.to_str()]") {
    std::string test_buff("schema://username:password@www.example.com:8080/hello/index.html");
    jdl::uri::schem schema_test(test_buff);

    CHECK(schema_test.to_str() == std::string("schema"));
  }

  SECTION("- case #2 - Without schema [schema.to_str()]") {
    std::string test_buff("username:password@www.example.com:8080/hello/index.html");
    jdl::uri::schem schema_test(test_buff);

    CHECK(schema_test.to_str() == std::string(""));
  }

  SECTION("- case #3 - Null string [schema.to_str()]") {
    std::string test_buff("");

    CHECK_THROWS_AS((jdl::uri::schem(test_buff)), std::invalid_argument);
    CHECK_THROWS_WITH((jdl::uri::schem(test_buff)), "buff_ref_.size() <= POSITION");
  }
}
