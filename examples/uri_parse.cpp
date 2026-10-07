/*
 *  Example: static-rule URI parsing with httpclientlite
 *
 *  The grammar (where each component starts and ends) is fixed at
 *  compile time — see include/httpclientlite/core/grammar.hpp.
 *  Extraction rules are static templates too — nothing is registered
 *  at runtime.
 */

#include <httpclientlite/core/parser.hpp>
#include <httpclientlite/core/rule.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
  const std::string uri = "https://user:secret@example.com:8443/images/logo.png?q=big&page=2#top";

  // 1. Parse — the engine walks the static rule tables
  core::parser p(uri);

  std::cout << "URI: " << p.uri() << "\n\n";
  std::cout << "scheme:   " << p.scheme().str() << "\n";
  std::cout << "userinfo: " << p.userinfo().str() << "\n";
  std::cout << "host:     " << p.host().str() << "\n";
  std::cout << "port:     " << p.port().str() << "\n";
  std::cout << "path:     " << p.path().str() << "\n";
  std::cout << "query:    " << p.query().str() << "\n";
  std::cout << "fragment: " << p.fragment().str() << "\n";

  // 2. Typed extraction — rules are static (compile-time location)
  std::cout << "\nStatic rules:\n";

  core::rule<std::string, core::location::host> domain("domain");
  std::cout << "domain:   " << domain.extract(p.get()).as_string() << "\n";

  core::rule<std::vector<std::string>, core::location::path> segments("segments", '/');
  core::param_value seg_val = segments.extract(p.get());
  std::cout << "segments:";
  for (const std::string& s : seg_val.as_list()) {
    std::cout << " [" << s << "]";
  }
  std::cout << "\n";

  core::rule<std::unordered_map<std::string, std::string>, core::location::query> params("params");
  core::param_value params_val = params.extract(p.get());
  std::cout << "params:";
  for (const auto& kv : params_val.as_map()) {
    std::cout << " " << kv.first << "=" << kv.second;
  }
  std::cout << "\n";

  // 3. The same via factory helpers
  std::cout << "\nFactories:\n";
  auto proto = core::make_string_rule<core::location::scheme>("proto");
  std::cout << "proto:    " << proto->extract(p.get()).as_string() << "\n";

  return 0;
}
