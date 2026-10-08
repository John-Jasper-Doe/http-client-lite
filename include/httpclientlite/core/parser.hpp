
#pragma once

/// @file parser.hpp
/// @brief Universal URI parser with non-owning component views
///
/// Parses a URI string according to RFC3986 into its components.
/// Components are stored as string_chunk views (non-owning), so the
/// original URI string must outlive the parser instance.
///
/// URI structure: scheme:[//authority]path[?query][#fragment]
/// Authority:     [userinfo@]host[:port]

#include <cstddef>
#include <string>
#include <utility>

#include "../common/common.hpp"
#include "components.hpp"
#include "grammar.hpp"

/// @namespace core
namespace core {

  /// @class parser
  /// @brief Parses a URI string into its components according to RFC3986
  ///
  /// The parser owns the URI string and creates non-owning string_chunk views
  /// for each component. The parser must outlive any components obtained from it.
  ///
  /// Parsing is driven by the static rule tables in grammar.hpp — the rules
  /// are fixed at compile time, nothing is registered at runtime.
  ///
  /// Usage:
  /// @code
  ///   core::parser p("http://example.com:8080/path?q=1#frag");
  ///   auto& comp = p.get();
  ///   // comp.scheme == "http", comp.host == "example.com", etc.
  /// @endcode
  class parser {
  private:
    std::string uri_;
    components comp_;

    /// @brief Internal: parse the URI string into components
    void parse() {
      grammar::parse(uri_, comp_);
    }

  public:
    /// @brief Parse a URI string
    /// @param uri - the URI string to parse
    explicit parser(std::string uri)
      : uri_(std::move(uri)) {
      parse();
    }

    /// The parser owns views into its URI string; they would dangle
    /// after a copy or move.
    ~parser() = default;
    parser(const parser&) = delete;
    parser& operator=(const parser&) = delete;
    parser(parser&&) = delete;
    parser& operator=(parser&&) = delete;

    /// @brief Get all parsed components
    [[nodiscard]] const components& get() const noexcept {
      return comp_;
    }

    /// @brief Get the original URI string
    [[nodiscard]] const std::string& uri() const noexcept {
      return uri_;
    }

    /// @brief Access individual components
    [[nodiscard]] string_chunk<> scheme() const noexcept {
      return comp_.scheme;
    }
    [[nodiscard]] string_chunk<> userinfo() const noexcept {
      return comp_.userinfo;
    }
    [[nodiscard]] string_chunk<> host() const noexcept {
      return comp_.host;
    }
    [[nodiscard]] string_chunk<> port() const noexcept {
      return comp_.port;
    }
    [[nodiscard]] string_chunk<> path() const noexcept {
      return comp_.path;
    }
    [[nodiscard]] string_chunk<> query() const noexcept {
      return comp_.query;
    }
    [[nodiscard]] string_chunk<> fragment() const noexcept {
      return comp_.fragment;
    }
  };

}  // namespace core
