/*
 *  Example: bulk URI parsing — a workload for the analysis targets
 *  (profiler, sanitizer, valgrind).
 *
 *  Parses a fixed corpus of URIs many times over and accumulates a
 *  checksum of the extracted components, so the optimizer cannot elide
 *  the work. Run it under callgrind (make profiler) to see where the
 *  parse time goes, or under ASan/valgrind to watch the allocations.
 */

#include <cstddef>
#include <httpclientlite/core/parser.hpp>
#include <httpclientlite/core/rule.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
  const std::vector<std::string> corpus = {
    "https://user:secret@example.com:8443/images/logo.png?q=big&page=2#top",
    "http://localhost:8080/api/v1/users?id=42&active=true",
    "ftp://files.example.org/pub/releases/1.2.3/archive.tar.gz",
    "https://sub.domain.example.co.uk/path/to/resource.html",
    "http://192.168.1.1:8080/",
    "https://example.com?single",
    "https://example.com/deeply/nested/path/segments/for/splitting?a=1&b=2&c=3",
    "https://api.example.com/v2/items/98765/reviews?page=3&sort=recent#rev2",
  };

  core::rule<std::unordered_map<std::string, std::string>, core::location::query> params("params");

  constexpr std::size_t kIterations = 10000;

  std::size_t checksum = 0;
  for (std::size_t i = 0; i < kIterations; ++i) {
    for (const std::string& uri : corpus) {
      core::parser p(uri);
      checksum += p.host().size();
      checksum += p.path().size();
      checksum += p.scheme().size();
      checksum += params.extract(p.get()).as_map().size();
    }
  }

  std::cout << "Parsed " << kIterations * corpus.size() << " URIs," << " checksum: " << checksum
            << "\n";
  return 0;
}
