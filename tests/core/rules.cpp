
#include <catch2/catch_test_macros.hpp>
#include <httpclientlite/core/parser.hpp>
#include <httpclientlite/core/rule.hpp>

using core::location;
using core::param_value;
using core::parser;
using core::rule;

// =========================================================================
// param_value tests
// =========================================================================

TEST_CASE("param_value", "[param_value]") {
  SECTION("Default construction is empty") {
    param_value pv;

    CHECK(pv.type() == param_value::type_tag::empty);
    CHECK(pv.empty());
  }

  SECTION("String construction") {
    param_value pv("hello");

    CHECK(pv.type() == param_value::type_tag::string);
    CHECK_FALSE(pv.empty());
    CHECK(pv.as_string() == "hello");
  }

  SECTION("List construction") {
    std::vector<std::string> items = { "a", "b", "c" };
    param_value pv(items);

    CHECK(pv.type() == param_value::type_tag::list);
    CHECK_FALSE(pv.empty());
    CHECK(pv.as_list().size() == 3);
    CHECK(pv.as_list()[0] == "a");
    CHECK(pv.as_list()[2] == "c");
  }

  SECTION("Map construction") {
    std::unordered_map<std::string, std::string> m = { { "key", "value" } };
    param_value pv(m);

    CHECK(pv.type() == param_value::type_tag::map);
    CHECK_FALSE(pv.empty());
    CHECK(pv.as_map().at("key") == "value");
  }

  SECTION("Copy construction") {
    param_value pv1("test");
    param_value pv2(pv1);

    CHECK(pv2.as_string() == "test");
  }

  SECTION("Move construction") {
    param_value pv1("test");
    param_value pv2(std::move(pv1));

    CHECK(pv2.as_string() == "test");
  }
}

// =========================================================================
// rule<std::string, Loc> tests
// =========================================================================

TEST_CASE("rule<string> extracts components as strings", "[rule_string]") {
  SECTION("Extract scheme") {
    parser p("http://example.com");
    rule<std::string, location::scheme> r("protocol");

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::string);
    CHECK(val.as_string() == "http");
  }

  SECTION("Extract host") {
    parser p("http://example.com");
    rule<std::string, location::host> r("domain");

    param_value val = r.extract(p.get());

    CHECK(val.as_string() == "example.com");
  }

  SECTION("Extract port") {
    parser p("http://localhost:8080/api");
    rule<std::string, location::port> r("port");

    param_value val = r.extract(p.get());

    CHECK(val.as_string() == "8080");
  }

  SECTION("Extract path") {
    parser p("http://example.com/a/b/c");
    rule<std::string, location::path> r("path");

    param_value val = r.extract(p.get());

    CHECK(val.as_string() == "/a/b/c");
  }

  SECTION("Extract fragment") {
    parser p("http://example.com/page#section");
    rule<std::string, location::fragment> r("anchor");

    param_value val = r.extract(p.get());

    CHECK(val.as_string() == "section");
  }

  SECTION("Extract userinfo") {
    parser p("https://user:pass@example.com");
    rule<std::string, location::userinfo> r("auth");

    param_value val = r.extract(p.get());

    CHECK(val.as_string() == "user:pass");
  }

  SECTION("Empty component returns empty string") {
    parser p("http://example.com");
    rule<std::string, location::port> r("port");

    param_value val = r.extract(p.get());

    CHECK(val.as_string().empty());
  }
}

// =========================================================================
// rule<std::vector<std::string>, Loc> tests
// =========================================================================

TEST_CASE("rule<vector> splits components by delimiter", "[rule_list]") {
  SECTION("Split path by '/'") {
    parser p("http://example.com/a/b/c");
    rule<std::vector<std::string>, location::path> r("segments", '/');

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::list);
    // "/a/b/c" split by '/' gives ["", "a", "b", "c"]
    CHECK(val.as_list().size() == 4);
    CHECK(val.as_list()[0] == "");
    CHECK(val.as_list()[1] == "a");
    CHECK(val.as_list()[2] == "b");
    CHECK(val.as_list()[3] == "c");
  }

  SECTION("Split path with leading slash") {
    parser p("http://example.com/search");
    rule<std::vector<std::string>, location::path> r("segments", '/');

    param_value val = r.extract(p.get());

    // "/search" split by '/' gives ["", "search"]
    CHECK(val.as_list().size() == 2);
    CHECK(val.as_list()[0] == "");
    CHECK(val.as_list()[1] == "search");
  }

  SECTION("Empty component returns empty list") {
    parser p("http://example.com");
    rule<std::vector<std::string>, location::path> r("segments", '/');

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::list);
    CHECK(val.as_list().empty());
  }

  SECTION("Single element (no delimiter found)") {
    parser p("http://example.com/file.txt");
    rule<std::vector<std::string>, location::path> r("segments", '/');

    param_value val = r.extract(p.get());

    // "/file.txt" split by '/' gives ["", "file.txt"]
    CHECK(val.as_list().size() == 2);
    CHECK(val.as_list()[1] == "file.txt");
  }
}

// =========================================================================
// rule<map> tests (query string parsing)
// =========================================================================

TEST_CASE("rule<map> parses query strings", "[rule_map]") {
  SECTION("Parse simple query") {
    parser p("http://example.com/search?q=laptop&page=2");
    rule<std::unordered_map<std::string, std::string>, location::query> r("params");

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::map);
    CHECK(val.as_map().size() == 2);
    CHECK(val.as_map().at("q") == "laptop");
    CHECK(val.as_map().at("page") == "2");
  }

  SECTION("Parse empty query") {
    parser p("http://example.com/search?");
    rule<std::unordered_map<std::string, std::string>, location::query> r("params");

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::map);
    CHECK(val.as_map().empty());
  }

  SECTION("Parse query without ?") {
    parser p("http://example.com/search");
    rule<std::unordered_map<std::string, std::string>, location::query> r("params");

    param_value val = r.extract(p.get());

    CHECK(val.type() == param_value::type_tag::map);
    CHECK(val.as_map().empty());
  }

  SECTION("Parse query with empty value") {
    parser p("http://example.com?q=&sort=asc");
    rule<std::unordered_map<std::string, std::string>, location::query> r("params");

    param_value val = r.extract(p.get());

    CHECK(val.as_map().size() == 2);
    CHECK(val.as_map().at("q") == "");
    CHECK(val.as_map().at("sort") == "asc");
  }

  SECTION("Parse query with multiple same keys (last wins)") {
    parser p("http://example.com?tr=a&tr=b");
    rule<std::unordered_map<std::string, std::string>, location::query> r("params");

    param_value val = r.extract(p.get());

    // std::unordered_map overwrites duplicate keys
    CHECK(val.as_map().size() == 1);
    CHECK(val.as_map().at("tr") == "b");
  }
}

// =========================================================================
// Factory function tests
// =========================================================================

TEST_CASE("Factory functions create correct rules", "[factory]") {
  SECTION("make_string_rule") {
    auto r = core::make_string_rule<location::scheme>("protocol");

    parser p("http://example.com");
    param_value val = r->extract(p.get());

    CHECK(r->name() == "protocol");
    CHECK(val.as_string() == "http");
  }

  SECTION("make_list_rule") {
    auto r = core::make_list_rule<location::path>("segments", '/');

    parser p("http://example.com/a/b");
    param_value val = r->extract(p.get());

    CHECK(r->name() == "segments");
    CHECK(val.as_list().size() == 3);
  }

  SECTION("make_map_rule") {
    auto r = core::make_map_rule("params");

    parser p("http://example.com?q=1");
    param_value val = r->extract(p.get());

    CHECK(r->name() == "params");
    CHECK(val.as_map().at("q") == "1");
  }
}
