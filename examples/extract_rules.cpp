/*
 *  Example: extraction rules and string chunks — a workload for the
 *  analysis targets (profiler, sanitizer, valgrind).
 *
 *  Runs the static rule templates (string/list/map) and the non-owning
 *  string_chunk views over a URI in a loop: the rules allocate the
 *  extracted values (std::string, std::vector, std::unordered_map),
 *  the chunks only view the original string without copying it.
 */

#include <cstddef>
#include <httpclientlite/common/common.hpp>
#include <httpclientlite/core/parser.hpp>
#include <httpclientlite/core/rule.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
  const std::string uri = "https://user:secret@example.com:8443/a/b/c?q=big&page=2&tag=cpp#top";
  core::parser p(uri);

  core::rule<std::string, core::location::host> domain("domain");
  core::rule<std::vector<std::string>, core::location::path> segments("segments", '/');
  core::rule<std::unordered_map<std::string, std::string>, core::location::query> params("params");

  constexpr std::size_t kIterations = 10000;

  std::size_t total_segments = 0;
  std::size_t total_params = 0;
  for (std::size_t i = 0; i < kIterations; ++i) {
    const core::param_value seg_val = segments.extract(p.get());
    total_segments += seg_val.as_list().size();

    const core::param_value params_val = params.extract(p.get());
    total_params += params_val.as_map().size();
  }

  // string_chunk: non-owning views over the same string, no allocations.
  const std::string host = domain.extract(p.get()).as_string();
  const std::string path = p.path().str();
  const common::string_chunk<> chunk(path);
  std::size_t slashes = 0;
  for (std::size_t pos = chunk.find('/'); pos != common::string_chunk<>::npos;
       pos = chunk.find('/', pos + 1)) {
    ++slashes;
  }

  std::cout << "Extracted " << total_params << " params and" << " " << total_segments
            << " segments;" << " host: " << host << ", path slashes: " << slashes << "\n";
  return 0;
}
