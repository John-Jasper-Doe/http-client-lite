#include "testhelper.hpp"

#include <jdl/httpclientlite.hpp>


TEST_CASE_MY("Test of method2string", "[http_client]") {
  using namespace jdl::http;

  std::string actual = method2string<method::GET>();
  ASSERT_STREQUAL(actual, std::string("GET"));
}


TEST_CASE_MY("Test of request", "[http_client]") {
  using namespace jdl::http;

  std::string expect_data{ "GET /127.0.0.1:8080 HTTP/1.1\r\n"
                           "Host: :80\r\n"
                           "Accept: */*\r\n"
                           "Content-Type: text/plain; version=0.0.4; charset=utf-8\r\n"
                           "Content-Length: 0\r\n\r\n"
                         };

  std::string actual = client::gen_request<method::GET>(jdl::uri::uri("http://127.0.0.1:8080"));
  ASSERT_EQUAL(std::strcmp(actual.c_str(), expect_data.c_str()), 0);
}
