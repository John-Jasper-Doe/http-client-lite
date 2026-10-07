
#include <catch2/catch_test_macros.hpp>
#include <httpclientlite/core/parser.hpp>

using core::components;
using core::parser;

TEST_CASE("URI Parser", "[parser]") {
  SECTION("Basic HTTP URL (scheme + host only)") {
    parser p("http://example.com");

    CHECK(p.scheme().str() == "http");
    CHECK(p.host().str() == "example.com");
    CHECK(p.port().empty());
    CHECK(p.path().empty());
    CHECK(p.query().empty());
    CHECK(p.fragment().empty());
    CHECK(p.userinfo().empty());
  }

  SECTION("HTTPS with path") {
    parser p("https://www.example.com/images/logo.png");

    CHECK(p.scheme().str() == "https");
    CHECK(p.host().str() == "www.example.com");
    CHECK(p.path().str() == "/images/logo.png");
  }

  SECTION("FTP URL") {
    parser p("ftp://ftp.example.com/pub/documents/report.pdf");

    CHECK(p.scheme().str() == "ftp");
    CHECK(p.host().str() == "ftp.example.com");
    CHECK(p.path().str() == "/pub/documents/report.pdf");
  }

  SECTION("Local file URI") {
    parser p("file:///C:/Users/Admin/Documents/data.txt");

    CHECK(p.scheme().str() == "file");
    CHECK(p.host().empty());
    CHECK(p.path().str() == "/C:/Users/Admin/Documents/data.txt");
  }

  SECTION("URN (no authority)") {
    parser p("urn:isbn:0451450523");

    CHECK(p.scheme().str() == "urn");
    CHECK(p.host().empty());
    CHECK(p.path().str() == "isbn:0451450523");
  }

  SECTION("URL with port") {
    parser p("http://localhost:8080/api/v1/status");

    CHECK(p.scheme().str() == "http");
    CHECK(p.host().str() == "localhost");
    CHECK(p.port().str() == "8080");
    CHECK(p.path().str() == "/api/v1/status");
  }

  SECTION("URL with fragment") {
    parser p("https://docs.example.com/guide/intro#authentication");

    CHECK(p.scheme().str() == "https");
    CHECK(p.host().str() == "docs.example.com");
    CHECK(p.path().str() == "/guide/intro");
    CHECK(p.fragment().str() == "authentication");
  }

  SECTION("URL with query string") {
    parser p("https://shop.example.com/search?q=laptop&sort=price_asc&page=2");

    CHECK(p.scheme().str() == "https");
    CHECK(p.host().str() == "shop.example.com");
    CHECK(p.path().str() == "/search");
    CHECK(p.query().str() == "q=laptop&sort=price_asc&page=2");
  }

  SECTION("Mailto scheme") {
    parser p("mailto:support@example.com?subject=Help&body=Hello");

    CHECK(p.scheme().str() == "mailto");
    CHECK(p.path().str() == "support@example.com");
    CHECK(p.query().str() == "subject=Help&body=Hello");
  }

  SECTION("Tel scheme") {
    parser p("tel:+1-800-555-0199;ext=123");

    CHECK(p.scheme().str() == "tel");
    CHECK(p.path().str() == "+1-800-555-0199;ext=123");
  }

  SECTION("Data URI") {
    parser p("data:image/png;base64,iVBORw0KGgo=");

    CHECK(p.scheme().str() == "data");
    CHECK(p.path().str() == "image/png;base64,iVBORw0KGgo=");
  }

  SECTION("URN UUID") {
    parser p("urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");

    CHECK(p.scheme().str() == "urn");
    CHECK(p.path().str() == "uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");
  }

  SECTION("IPv6 address") {
    parser p("http://[2001:db8:85a3::8a2e:370:7334]:8080/index.html");

    CHECK(p.scheme().str() == "http");
    CHECK(p.host().str() == "2001:db8:85a3::8a2e:370:7334");
    CHECK(p.port().str() == "8080");
    CHECK(p.path().str() == "/index.html");
  }

  SECTION("URL with percent-encoding (structure only)") {
    parser p(
    "https://example.com/search?q=%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82%20%D0%BC%D0%B8%D1%80");

    CHECK(p.scheme().str() == "https");
    CHECK(p.host().str() == "example.com");
    CHECK(p.path().str() == "/search");
    CHECK(p.query().str() == "q=%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82%20%D0%BC%D0%B8%D1%80");
  }

  SECTION("URL with userinfo") {
    parser p("https://admin:SuperSecretPass123@api.example.com/v1/data");

    CHECK(p.scheme().str() == "https");
    CHECK(p.userinfo().str() == "admin:SuperSecretPass123");
    CHECK(p.host().str() == "api.example.com");
    CHECK(p.path().str() == "/v1/data");
  }

  SECTION("Magnet link (complex query)") {
    parser p(
    "magnet:?xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a&dn=My+File&tr=udp://"
    "tracker.example.com:80");

    CHECK(p.scheme().str() == "magnet");
    CHECK(p.query().str() ==
          "xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a&dn=My+File&tr=udp://"
          "tracker.example.com:80");
  }

  SECTION("Empty URI") {
    parser p("");

    CHECK(p.get().empty());
  }

  SECTION("Scheme only") {
    parser p("http:");

    CHECK(p.scheme().str() == "http");
    CHECK_FALSE(p.get().empty());  // scheme is not empty
  }

  SECTION("components::empty() returns true when all empty") {
    parser p("");
    CHECK(p.get().empty());
  }

  SECTION("components::empty() returns false when scheme present") {
    parser p("http:");
    CHECK_FALSE(p.get().empty());
  }
}

TEST_CASE("Sequential grammar: fragment before query", "[grammar]") {
  parser p("x:p#a?q");

  CHECK(p.scheme().str() == "x");
  CHECK(p.path().str() == "p");
  CHECK(p.query().empty());
  CHECK(p.fragment().str() == "a?q");
}
