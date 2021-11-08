#include "testhelper.hpp"

#include <jdl/httpclientlite.hpp>


TEST_CASE_MY("Test of uri.constructor1", "[uri]") {
  jdl::uri uri("http://ru.example.org");

  ASSERT_STREQUAL(uri.protocol, "http");
  ASSERT_STREQUAL(uri.address, "ru.example.org");
  ASSERT_STREQUAL(uri.port, "80");
//  ASSERT_STREQUAL(uri.hash, "example.org");
//  ASSERT_STREQUAL(uri.host, "example.org");
//  ASSERT_STREQUAL(uri.querystring, "example.org");
}

TEST_CASE_MY("Test of uri.constructor2", "[uri]") {
  jdl::uri uri("127.0.0.1:8080");

  ASSERT_STREQUAL(uri.protocol, "");
//  ASSERT_STREQUAL(uri.address, "127.0.0.1");
//  ASSERT_STREQUAL(uri.port, "8080");
//  ASSERT_STREQUAL(uri.host, "");
//  ASSERT_STREQUAL(uri.query_string, "");
}

TEST_CASE_MY("Test of uri.constructor3", "[uri]") {
  jdl::uri uri("https://duckduckgo.com/?q=gcc+warning+switch+off&t=lm&atb=v208-1&ia=web&iax=qa");

  ASSERT_STREQUAL(uri.protocol, "http");
  ASSERT_STREQUAL(uri.address, "ru.example.org");
  ASSERT_STREQUAL(uri.port, "80");
//  ASSERT_STREQUAL(uri.hash, "example.org");
//  ASSERT_STREQUAL(uri.host, "example.org");
//  ASSERT_STREQUAL(uri.querystring, "example.org");
}
