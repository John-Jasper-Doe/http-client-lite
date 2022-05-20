#include <jdl/httpclientlite.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


using jdl::details_http_client_lite::offseter;
using jdl::details_http_client_lite::buffer;
using jdl::details_http_client_lite::string_chunk_t;



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
    buffer  buff("Hello, world!");
    buffer  buff_empty("");

    offseter<> offseter_1(0, 5);
    REQUIRE( offseter_1.offset() == 0);
    REQUIRE( offseter_1.size() == 5);
    REQUIRE(!offseter_1.is_empty());
    REQUIRE( offseter_1.get(buff) == string_chunk_t(buff.data(), 5));
    REQUIRE( offseter_1.get(buff_empty) == string_chunk_t());

    offseter<> offseter_2(2, 4);
    REQUIRE( offseter_2.offset() == 2);
    REQUIRE( offseter_2.size() == 4);
    REQUIRE(!offseter_2.is_empty());
    REQUIRE( offseter_2.get(buff) == string_chunk_t(buff.data() + 2, 4));
    REQUIRE( offseter_2.get(buff_empty) == string_chunk_t());

    REQUIRE(offseter_1 == offseter_1);
    REQUIRE(offseter_1 != offseter_2);

    offseter_1.set(offseter_2.offset(),
                   static_cast<typename offseter<uint16_t>::internel_size_type>(offseter_2.size()));
  }
}


TEST_CASE("Test JDL of Buffer", "[buffer]") {
  buffer buff({ "First initialized" , 17 });

  SECTION("Internel metodes") {
    REQUIRE(!buff.is_empty());
    REQUIRE( buff.size() == 17);
    REQUIRE( buff.data() == std::string({ "First initialized" , 17 }));

    REQUIRE(buff.template get_offseter<uint16_t>("") == offseter<uint16_t>());
    REQUIRE(buff.template get_offseter<uint16_t>("iii") == offseter<uint16_t>());
    REQUIRE(buff.template get_offseter<uint16_t>("irst") == offseter<uint16_t>(1, 4));

    jdl::details_http_client_lite::checker_t<string_chunk_t> c;
    REQUIRE(buff.template get_token<uint16_t, 6, 4>("i", c) == offseter<uint16_t>());
  }
}

