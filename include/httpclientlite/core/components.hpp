
#pragma once

/// @file components.hpp
/// @brief Shared URI vocabulary types and chunk helpers
///
/// Defines the parsed-component views, the component-location enum,
/// and the low-level string_chunk helpers shared by the parser, the
/// grammar tables and the extraction rules. This header has no
/// dependencies on parser, grammar or rule.

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/common.hpp"

/// @namespace core
namespace core {

  using common::string_chunk;

  /// @brief Identifies which component of the URI to extract
  enum class location {
    scheme,     ///< Protocol (e.g., "http", "https", "ftp")
    authority,  ///< [userinfo@]host[:port] region — never stored as a
                ///< whole, only split into userinfo/host/port
    userinfo,   ///< User information before @ (e.g., "user:pass")
    host,       ///< Hostname or IP address
    port,       ///< Port number as string
    path,       ///< Resource path (e.g., "/images/logo.png")
    query,      ///< Query string after ? (e.g., "q=test&page=1")
    fragment    ///< Fragment after # (e.g., "section1")
  };

  namespace detail {

    /// @brief Find first occurrence of a character in a string_chunk
    /// @param chunk - the chunk to search in
    /// @param c - character to find
    /// @param pos - start position
    /// @return Position of the character, or npos if not found
    inline std::size_t find_char(const string_chunk<>& chunk, char c, std::size_t pos = 0) {
      if (pos >= chunk.size()) {
        return string_chunk<>::npos;
      }
      const char* data = chunk.data();
      const char* end = data + chunk.size();
      for (const char* it = data + pos; it != end; ++it) {
        if (*it == c) {
          return static_cast<std::size_t>(it - data);
        }
      }
      return string_chunk<>::npos;
    }

    /// @brief Split a string_chunk by a delimiter character
    /// @param chunk - the chunk to split
    /// @param delimiter - character to split on
    /// @return Vector of substrings (empty if chunk is empty)
    inline std::vector<std::string> split(const string_chunk<>& chunk, char delimiter) {
      std::vector<std::string> result;
      if (chunk.empty()) {
        return result;
      }

      std::size_t pos = 0;
      while (pos < chunk.size()) {
        std::size_t next = find_char(chunk, delimiter, pos);
        if (next == string_chunk<>::npos) {
          result.push_back(chunk.substr(pos).str());
          break;
        }
        result.push_back(chunk.substr(pos, next - pos).str());
        pos = next + 1;
      }
      return result;
    }

    /// @brief Parse a query string into a key-value map
    /// @param query - the query string (e.g., "q=test&page=1")
    /// @param kv_delim - delimiter between key and value (default '=')
    /// @param pair_delim - delimiter between pairs (default '&')
    /// @return Map of key-value pairs
    inline std::unordered_map<std::string, std::string> parse_query(const string_chunk<>& query,
                                                                    char kv_delim = '=',
                                                                    char pair_delim = '&') {
      std::unordered_map<std::string, std::string> result;
      if (query.empty()) {
        return result;
      }

      std::size_t pos = 0;
      while (pos < query.size()) {
        std::size_t next = find_char(query, pair_delim, pos);

        string_chunk<> pair =
        (next == string_chunk<>::npos) ? query.substr(pos) : query.substr(pos, next - pos);

        std::size_t eq = find_char(pair, kv_delim);
        if (eq != string_chunk<>::npos) {
          std::string key = pair.substr(0, eq).str();
          std::string value = pair.substr(eq + 1).str();
          result[key] = value;
        }

        if (next == string_chunk<>::npos) {
          break;
        }
        pos = next + 1;
      }
      return result;
    }

  }  // namespace detail

  /// @struct components
  /// @brief Non-owning views of all URI components
  ///
  /// Each member is a string_chunk pointing into the parser's URI string.
  /// All members are empty if the corresponding component is absent.
  struct components {
    string_chunk<> scheme;    ///< e.g., "http"
    string_chunk<> userinfo;  ///< e.g., "user:pass"
    string_chunk<> host;      ///< e.g., "example.com"
    string_chunk<> port;      ///< e.g., "8080"
    string_chunk<> path;      ///< e.g., "/images/logo.png"
    string_chunk<> query;     ///< e.g., "q=test&page=1"
    string_chunk<> fragment;  ///< e.g., "section1"

    /// @brief Returns true if ALL components are empty
    bool empty() const noexcept {
      return scheme.empty() && userinfo.empty() && host.empty() && port.empty() && path.empty() &&
             query.empty() && fragment.empty();
    }
  };

}  // namespace core
