/*
 *  example for httpclientlite.hxx
 */

#include <iostream>
#include <jdl/httpclientlite.hpp>
#include <map>
#include <string>

using namespace jdl;

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << "Use " << argv[0] << " http://example.org" << std::endl;
    return EXIT_SUCCESS;
  }

  http::response response = HTTPClient::request<http::method::POST>(uri::uri(argv[1]));

  if (response.status == http::resp_stat::failure) {
    std::cout << "Request failed!" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Request success" << std::endl;

  std::cout << "Server protocol: " << response.protocol << std::endl;
  std::cout << "Response code: " << response.resp << std::endl;
  std::cout << "Response string: " << response.resp_str << std::endl;

  std::cout << "Headers:" << std::endl;

  for (string_map_t::iterator it = response.header.begin(); it != response.header.end(); ++it) {
    std::cout << "\t" << it->first << "=" << it->second << std::endl;
  }

  std::cout << response.body << std::endl;

  return EXIT_SUCCESS;
}
