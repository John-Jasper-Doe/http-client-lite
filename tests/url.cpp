#include <jdl/httpclientlite.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace jdl;

TEST_CASE("Test JDL of URL", "[uri]") {
  SECTION("First test") {
    uri::url url("https://google.com");

    REQUIRE(url.schem() == std::string("https"));
  }
}
