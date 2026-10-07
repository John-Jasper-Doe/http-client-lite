
#include <catch2/catch_test_macros.hpp>
#include <httpclientlite/common/common.hpp>

using chunk_t = common::string_chunk<>;
using common::string_chunk;

TEST_CASE("Test JDL of String Chunk", "[string_chunk]") {
  SECTION("Internal of types") {
    REQUIRE(std::is_same<typename chunk_t::value_type, char>::value);

    REQUIRE(std::is_same<typename chunk_t::pointer, char*>::value);
    REQUIRE(std::is_same<typename chunk_t::const_pointer, char const*>::value);
    REQUIRE(std::is_same<typename chunk_t::reference, char&>::value);
    REQUIRE(std::is_same<typename chunk_t::const_reference, char const&>::value);

    REQUIRE(std::is_same<typename chunk_t::iterator, char const*>::value);
    REQUIRE(std::is_same<typename chunk_t::const_iterator, char const*>::value);
    REQUIRE(std::is_same<typename chunk_t::reverse_iterator,
                         typename std::reverse_iterator<char const*>>::value);
    REQUIRE(std::is_same<typename chunk_t::const_reverse_iterator,
                         typename std::reverse_iterator<char const*>>::value);

    REQUIRE(std::is_same<typename chunk_t::size_type, std::size_t>::value);
    REQUIRE(std::is_same<typename chunk_t::difference_type, std::ptrdiff_t>::value);
  }

  SECTION("String_chunk of constructed by default") {
    chunk_t ch;

    REQUIRE(ch.data() == nullptr);
    REQUIRE(ch.size() == 0);

    REQUIRE(ch.begin() == nullptr);
    REQUIRE(ch.end() == nullptr);
    REQUIRE(ch.begin() == ch.end());

    REQUIRE(ch.cbegin() == nullptr);
    REQUIRE(ch.cend() == nullptr);
    REQUIRE(ch.cbegin() == ch.cend());

    REQUIRE(ch.empty());
  }

  SECTION("String_chunk of constructed") {
    std::string test_string("Hello word!", 11);
    chunk_t ch1(test_string);

    REQUIRE(ch1.data() == test_string.data());
    REQUIRE(ch1.size() == 11);

    REQUIRE(ch1.begin() == test_string.data());
    REQUIRE(ch1.end() == test_string.data() + test_string.size());

    REQUIRE(!ch1.empty());

    chunk_t ch2(ch1);

    REQUIRE(ch2.data() == test_string.data());
    REQUIRE(ch2.size() == 11);

    REQUIRE(ch2.begin() == test_string.data());
    REQUIRE(ch2.end() == test_string.data() + test_string.size());

    REQUIRE(!ch2.empty());

    char const* test_const_string = "Hi!!!";
    chunk_t ch3(test_const_string, std::strlen(test_const_string));

    REQUIRE(ch3.data() == test_const_string);
    REQUIRE(ch3.size() == 5);

    REQUIRE(ch3.begin() == test_const_string);
    REQUIRE(ch3.end() == test_const_string + std::strlen(test_const_string));

    REQUIRE(!ch3.empty());
  }

  SECTION("String_chunk. Testing a function of find") {
    std::string test_string("Hello word!", 11);
    chunk_t ch(test_string);

    REQUIRE(!ch.empty());
    REQUIRE(ch.find("llo") == 2);
    REQUIRE(ch.find("") == 0);
    REQUIRE(ch.find("zzz") == chunk_t::npos);
    REQUIRE(ch.find("Hello word!!") == chunk_t::npos);

    REQUIRE(ch.find("llo", 2) == 2);
    REQUIRE(ch.find("", 2) == 2);
    REQUIRE(ch.find("zzz", 2) == chunk_t::npos);
    REQUIRE(ch.find("Hello word!!", 2) == chunk_t::npos);
  }

  SECTION("String_chunk. Testing the element access function") {
    std::string test_string("Hello word!", 11);
    chunk_t ch(test_string);

    REQUIRE(!ch.empty());
    REQUIRE(ch[0] == 'H');
    REQUIRE(ch[6] == 'w');
  }

  SECTION("String_chunk. Testing the assignment function") {
    std::string test_string("Hello word!", 11);
    chunk_t ch1(test_string);
    chunk_t ch2;

    REQUIRE(!ch1.empty());
    REQUIRE(ch2.empty());

    ch2 = ch1;

    REQUIRE(!ch1.empty());
    REQUIRE(!ch2.empty());
    REQUIRE(ch1.data() == ch2.data());
    REQUIRE(ch1.size() == ch2.size());
  }

  SECTION("operator== and operator!=") {
    std::string str1("Hello", 5);
    std::string str2("Hello", 5);
    std::string str3("World", 5);

    chunk_t ch1(str1);
    chunk_t ch2(str2);
    chunk_t ch3(str3);

    // Same content, different pointers — EQUAL (content-based comparison)
    REQUIRE(ch1 == ch2);

    // Different content — NOT equal
    REQUIRE(ch1 != ch3);

    // Self-comparison
    REQUIRE(ch1 == ch1);

    // Two empty chunks
    chunk_t empty1;
    chunk_t empty2;
    REQUIRE(empty1 == empty2);
  }

  SECTION("str() converts to std::string") {
    std::string test_string("Hello", 5);
    chunk_t ch(test_string);

    std::string result = ch.str();

    REQUIRE(result == "Hello");
    REQUIRE(result.size() == 5);
  }

  SECTION("str() on empty chunk returns empty string") {
    chunk_t ch;

    std::string result = ch.str();

    REQUIRE(result.empty());
  }

  SECTION("at() returns correct element") {
    std::string test_string("ABCDE", 5);
    chunk_t ch(test_string);

    REQUIRE(ch.at(0) == 'A');
    REQUIRE(ch.at(2) == 'C');
    REQUIRE(ch.at(4) == 'E');
  }

  SECTION("substr() returns correct sub-range") {
    std::string test_string("Hello world!", 12);
    chunk_t ch(test_string);

    chunk_t sub = ch.substr(6, 5);

    REQUIRE(sub.size() == 5);
    REQUIRE(sub[0] == 'w');
    REQUIRE(sub[4] == 'd');
  }

  SECTION("substr() with default count takes rest of string") {
    std::string test_string("Hello world!", 12);
    chunk_t ch(test_string);

    chunk_t sub = ch.substr(6);

    REQUIRE(sub.size() == 6);
    REQUIRE(sub[0] == 'w');
  }

  SECTION("substr() beyond size returns empty chunk") {
    std::string test_string("Hi", 2);
    chunk_t ch(test_string);

    chunk_t sub = ch.substr(10);

    REQUIRE(sub.empty());
  }

  SECTION("substr() count exceeding size is clamped") {
    std::string test_string("Hello", 5);
    chunk_t ch(test_string);

    chunk_t sub = ch.substr(2, 100);

    REQUIRE(sub.size() == 3);
    REQUIRE(sub[0] == 'l');
    REQUIRE(sub[2] == 'o');
  }

  SECTION("find() on empty chunk returns npos") {
    chunk_t ch;

    REQUIRE(ch.find("abc") == chunk_t::npos);
  }

  SECTION("find() with pos beyond size returns npos") {
    std::string test_string("Hi", 2);
    chunk_t ch(test_string);

    REQUIRE(ch.find("i", 5) == chunk_t::npos);
  }

  SECTION("Reverse iterators go in reverse order") {
    std::string test_string("ABC", 3);
    chunk_t ch(test_string);

    std::string reversed;
    for (auto it = ch.rbegin(); it != ch.rend(); ++it) {
      reversed += *it;
    }

    REQUIRE(reversed == "CBA");
  }

  SECTION("crbegin and crend same as rbegin and rend") {
    std::string test_string("ABC", 3);
    chunk_t ch(test_string);

    REQUIRE(ch.crbegin() == ch.rbegin());
    REQUIRE(ch.crend() == ch.rend());
  }

  SECTION("find() returns npos when pattern not found") {
    std::string test_string("Hello", 5);
    chunk_t ch(test_string);

    REQUIRE(ch.find("xyz") == chunk_t::npos);
  }
}
