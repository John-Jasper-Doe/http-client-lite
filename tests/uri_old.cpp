#include <jdl/httpclientlite.hpp>

#include "testhelper.hpp"

TEST_CASE_MY("Test of uri.constructor1", "[uri]") {
  //  jdl::uri::uri uri("http://ru.example.org");

  //  ASSERT_STREQUAL(uri.protocol, "http");
  //  ASSERT_STREQUAL(uri.address, "ru.example.org");
  //  ASSERT_STREQUAL(uri.port, "80");
  //  ASSERT_STREQUAL(uri.hash, "example.org");
  //  ASSERT_STREQUAL(uri.host, "example.org");
  //  ASSERT_STREQUAL(uri.querystring, "example.org");
}

TEST_CASE_MY("Test of uri.constructor2", "[uri]") {
  //  jdl::uri::uri uri("127.0.0.1:8080");

  //  ASSERT_STREQUAL(uri.protocol, "");
  //  ASSERT_STREQUAL(uri.address, "127.0.0.1");
  //  ASSERT_STREQUAL(uri.port, "8080");
  //  ASSERT_STREQUAL(uri.host, "");
  //  ASSERT_STREQUAL(uri.query_string, "");
}

TEST_CASE_MY("Test of uri.constructor3", "[uri]") {
  //  jdl::uri::uri
  //  uri("https://duckduckgo.com/?q=gcc+warning+switch+off&t=lm&atb=v208-1&ia=web&iax=qa");

  //  ASSERT_STREQUAL(uri.protocol, "http");
  //  ASSERT_STREQUAL(uri.address, "ru.example.org");
  //  ASSERT_STREQUAL(uri.port, "80");
  //  ASSERT_STREQUAL(uri.hash, "example.org");
  //  ASSERT_STREQUAL(uri.host, "example.org");
  //  ASSERT_STREQUAL(uri.querystring, "example.org");
}

TEST_CASE_MY("Test of \"Scheme\" class", "[uri]") {
  jdl::uri::scheme scheme1("ftp");
  jdl::uri::scheme scheme2;
  jdl::uri::scheme scheme3;
  jdl::uri::scheme scheme4;
  jdl::uri::scheme scheme5;

  scheme2.parse<0>(
    "https://duckduckgo.com/?q=gcc+warning+switch+off&t=lm&atb=v208-1&ia=web&iax=qa");
  scheme3.parse<0>("abcdefghijklmnopqrstuvwxyz");
  scheme5.parse<2>(
    "https://duckduckgo.com/?q=gcc+warning+switch+off&t=lm&atb=v208-1&ia=web&iax=qa");

  ASSERT_STREQUAL(scheme1.name(), "ftp");
  ASSERT_STREQUAL(scheme2.name(), "https");
  ASSERT_STREQUAL(scheme3.name(), "");
  ASSERT_STREQUAL(scheme4.name(), "");
  ASSERT_STREQUAL(scheme5.name(), "tps");
}

TEST_CASE_MY("Test of \"Userinfo\" class", "[uri]") {
  jdl::uri::authority::userinfo userinfo1("user", "pass");
  jdl::uri::authority::userinfo userinfo2;
  jdl::uri::authority::userinfo userinfo3;
  jdl::uri::authority::userinfo userinfo4;
  jdl::uri::authority::userinfo userinfo5;

  userinfo2.parse<6>("ftp://user2:pass2@duckduckgo.com");
  userinfo3.parse<0>("abcdefghijklmnopqrstuvwxyz");
  userinfo5.parse<6>("ftp://user5@duckduckgo.com");

  ASSERT_STREQUAL(userinfo1.name(), "user");
  ASSERT_STREQUAL(userinfo1.pass(), "pass");
  ASSERT_STREQUAL(userinfo2.name(), "user2");
  ASSERT_STREQUAL(userinfo2.pass(), "pass2");
  ASSERT_STREQUAL(userinfo3.name(), "");
  ASSERT_STREQUAL(userinfo3.pass(), "");
  ASSERT_STREQUAL(userinfo4.name(), "");
  ASSERT_STREQUAL(userinfo4.pass(), "");
  ASSERT_STREQUAL(userinfo5.name(), "user5");
  ASSERT_STREQUAL(userinfo5.pass(), "");
}
