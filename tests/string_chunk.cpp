#include <jdl/httpclientlite.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


using jdl::details_http_client_lite::string_chunk_t;


TEST_CASE("Test JDL of String Chunk", "[string_chunk]") {
  SECTION("Internal of types") {
    REQUIRE(jdl::is_same_v<typename string_chunk_t::value_type, char>);

    REQUIRE(jdl::is_same_v<typename string_chunk_t::pointer, char*>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::const_pointer, char const*>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::reference, char&>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::const_reference, char const&>);

    REQUIRE(jdl::is_same_v<typename string_chunk_t::iterator, char const*>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::const_iterator, char const*>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::reverse_iterator,
                           typename std::reverse_iterator<char const*>>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::const_reverse_iterator,
                           typename std::reverse_iterator<char const*>>);

    REQUIRE(jdl::is_same_v<typename string_chunk_t::size_type, std::size_t>);
    REQUIRE(jdl::is_same_v<typename string_chunk_t::difference_type, std::ptrdiff_t>);
  }

  SECTION("String_chunk of constructed by default") {
    string_chunk_t string_chunk;

    REQUIRE(string_chunk.data()   == nullptr);
    REQUIRE(string_chunk.size()   == 0);

    REQUIRE(string_chunk.begin()  == nullptr);
    REQUIRE(string_chunk.end()    == nullptr);
    REQUIRE(string_chunk.begin()  == string_chunk.end());

    REQUIRE(string_chunk.cbegin() == nullptr);
    REQUIRE(string_chunk.cend()   == nullptr);
    REQUIRE(string_chunk.cbegin() == string_chunk.cend());

    REQUIRE(string_chunk.empty());
  }

  SECTION("String_chunk of constructed") {
    std::string test_string("Hello word!", 11);
    string_chunk_t string_chunk1(test_string);

    REQUIRE(string_chunk1.data()  == test_string.data());
    REQUIRE(string_chunk1.size()  == 11);

    REQUIRE(string_chunk1.begin() == test_string.data());
    REQUIRE(string_chunk1.end()   == test_string.data() + test_string.size());

    REQUIRE(string_chunk1.begin() == test_string.data());
    REQUIRE(string_chunk1.end()   == test_string.data() + test_string.size());

    REQUIRE(!string_chunk1.empty());


    string_chunk_t string_chunk2(string_chunk1);

    REQUIRE(string_chunk2.data()  == test_string.data());
    REQUIRE(string_chunk2.size()  == 11);

    REQUIRE(string_chunk2.begin() == test_string.data());
    REQUIRE(string_chunk2.end()   == test_string.data() + test_string.size());

    REQUIRE(string_chunk2.begin() == test_string.data());
    REQUIRE(string_chunk2.end()   == test_string.data() + test_string.size());

    REQUIRE(!string_chunk2.empty());

    char const* test_const_string = "Hi!!!";
    string_chunk_t string_chunk3(test_const_string, std::strlen(test_const_string));

    REQUIRE(string_chunk3.data()  == test_const_string);
    REQUIRE(string_chunk3.size()  == 5);

    REQUIRE(string_chunk3.begin() == test_const_string);
    REQUIRE(string_chunk3.end()   == test_const_string + std::strlen(test_const_string));

    REQUIRE(string_chunk3.begin() == test_const_string);
    REQUIRE(string_chunk3.end()   == test_const_string + std::strlen(test_const_string));

    REQUIRE(!string_chunk3.empty());
  }

  SECTION("String_chunk. Testing a function of find") {
    std::string test_string("Hello word!", 11);
    string_chunk_t string_chunk(test_string);

    REQUIRE(!string_chunk.empty());
    REQUIRE(string_chunk.find( { "llo",           3 }    ) == 2);
    REQUIRE(string_chunk.find( { "",              0 }    ) == 0);
    REQUIRE(string_chunk.find( { "zzz",           3 }    ) == std::string::npos);
    REQUIRE(string_chunk.find( { "Hello word!!", 12 }    ) == std::string::npos);

    REQUIRE(string_chunk.find( { "llo",           3 }, 2 ) == 2);
    REQUIRE(string_chunk.find( { "",              0 }, 2 ) == 2);
    REQUIRE(string_chunk.find( { "zzz",           3 }, 2 ) == std::string::npos);
    REQUIRE(string_chunk.find( { "Hello word!!", 12 }, 2 ) == std::string::npos);
  }

  SECTION("String_chunk. Testing the element access function") {
    std::string test_string("Hello word!", 11);
    string_chunk_t string_chunk(test_string);

    REQUIRE(!string_chunk.empty());
    REQUIRE(string_chunk[0] == 'H');
    REQUIRE(string_chunk[6] == 'w');
  }

  SECTION("String_chunk. Testing the assignment function") {
    std::string test_string("Hello word!", 11);
    string_chunk_t string_chunk1(test_string);
    string_chunk_t string_chunk2;

    REQUIRE(!string_chunk1.empty());
    REQUIRE( string_chunk2.empty());

    string_chunk2 = string_chunk1;

    REQUIRE(!string_chunk1.empty());
    REQUIRE(!string_chunk2.empty());
    REQUIRE( string_chunk1.data() == string_chunk2.data());
    REQUIRE( string_chunk1.size() == string_chunk2.size());
  }
}

