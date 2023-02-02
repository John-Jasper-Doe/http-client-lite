#include <jdl/httpclientlite.hpp>
#include <sstream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using jdl::common::offseter;

TEST_CASE("Test JDL of Offseter", "[offseter]") {
  SECTION("Internal of types") {
    CHECK(std::is_same<typename offseter<>::offset_type, uint16_t>::value);
    CHECK(std::is_same<typename offseter<>::size_type, size_t>::value);
    CHECK(std::is_same<typename offseter<>::internel_size_type, uint16_t>::value);

    CHECK(std::is_same<typename offseter<uint8_t>::offset_type, uint8_t>::value);
    CHECK(std::is_same<typename offseter<uint8_t>::size_type, size_t>::value);
    CHECK(std::is_same<typename offseter<uint8_t>::internel_size_type, uint8_t>::value);

    CHECK(std::is_same<typename offseter<uint32_t>::offset_type, uint32_t>::value);
    CHECK(std::is_same<typename offseter<uint32_t>::size_type, size_t>::value);
    CHECK(std::is_same<typename offseter<uint32_t>::internel_size_type, uint32_t>::value);
  }

  SECTION("Empty offseter") {
    // Check of constructor by default
    offseter<> offseter;

    CHECK(offseter.offset() == 0);
    CHECK(offseter.size() == 0);
    CHECK(offseter.is_empty());

    // Check of set function
    offseter.set(static_cast<jdl::common::offseter<>::offset_type>(30),
                 static_cast<jdl::common::offseter<>::internel_size_type>(3));

    CHECK(offseter.offset() == 30);
    CHECK(offseter.size() == 3);
    CHECK_FALSE(offseter.is_empty());
  }

  SECTION("Internel metodes of offseter") {
    offseter<> offseter_1(0, 5);
    CHECK(offseter_1.offset() == 0);
    CHECK(offseter_1.size() == 5);
    CHECK(!offseter_1.is_empty());

    offseter<> offseter_2(2, 4);
    CHECK(offseter_2.offset() == 2);
    CHECK(offseter_2.size() == 4);
    CHECK(!offseter_2.is_empty());

    // Check of operators
    CHECK(offseter_1 == offseter_1);
    CHECK(offseter_1 != offseter_2);

    // Check of set function
    offseter_1.set(offseter_2.offset(),
                   static_cast<typename offseter<>::internel_size_type>(offseter_2.size()));

    CHECK(offseter_1 == offseter_2);

    // Check of outputing stream operator
    std::stringstream str_out;

    str_out << offseter_2;
    CHECK(str_out.str() == std::string("[ offset: 2, size: 4 ]"));
  }
}
