/*
 *  example for httpclientlite.hxx
 */

#include <iostream>
#include <jdl/httpclientlite.hpp>
#include <map>
#include <string>

using namespace jdl;
using namespace jdl::details_http_client_lite;

int main() {

  buffer_t buff("http://");
  uri::schem schem(buff);

  std::cout << "schem: " << schem.to_str() << std::endl;


  using check_pack_t = details_http_client_lite::checker<uri::verify_cond_digit>;

  std::string test_str_false("http://");
  string_chunk_t string_chunk_false(test_str_false);

  std::cout << "string_chunk_false: " << (check_pack_t()(string_chunk_false) ? "TRUE" : "FALSE") << std::endl;

  std::string test_str_true("11111");
  string_chunk_t string_chunk_true(test_str_true);

  std::cout << "string_chunk_true: " << (check_pack_t()(string_chunk_true) ? "TRUE" : "FALSE") << std::endl;


  return EXIT_SUCCESS;
}
