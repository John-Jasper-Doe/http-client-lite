#include <jdl/httpclientlite.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using jdl::common::offseter;

TEST_CASE("Test JDL of Offseter", "[offseter]") {
  SECTION("Internal of types") {
    REQUIRE(std::is_same<typename offseter<>::offset_type, uint16_t>::value);
    REQUIRE(std::is_same<typename offseter<>::size_type, size_t>::value);
    REQUIRE(std::is_same<typename offseter<>::internel_size_type, uint16_t>::value);

    REQUIRE(std::is_same<typename offseter<uint8_t>::offset_type, uint8_t>::value);
    REQUIRE(std::is_same<typename offseter<uint8_t>::size_type, size_t>::value);
    REQUIRE(std::is_same<typename offseter<uint8_t>::internel_size_type, uint8_t>::value);

    REQUIRE(std::is_same<typename offseter<uint32_t>::offset_type, uint32_t>::value);
    REQUIRE(std::is_same<typename offseter<uint32_t>::size_type, size_t>::value);
    REQUIRE(std::is_same<typename offseter<uint32_t>::internel_size_type, uint32_t>::value);
  }

  SECTION("Empty offseter") {
    offseter<> offseter;

    REQUIRE(offseter.offset() == 0);
    REQUIRE(offseter.size() == 0);
    REQUIRE(offseter.is_empty());
  }

  SECTION("Internel metodes of offseter") {
    offseter<> offseter_1(0, 5);
    REQUIRE(offseter_1.offset() == 0);
    REQUIRE(offseter_1.size() == 5);
    REQUIRE(!offseter_1.is_empty());

    offseter<> offseter_2(2, 4);
    REQUIRE(offseter_2.offset() == 2);
    REQUIRE(offseter_2.size() == 4);
    REQUIRE(!offseter_2.is_empty());

    REQUIRE(offseter_1 == offseter_1);
    REQUIRE(offseter_1 != offseter_2);

    offseter_1.set(offseter_2.offset(),
                   static_cast<typename offseter<uint16_t>::internel_size_type>(offseter_2.size()));
  }
}
