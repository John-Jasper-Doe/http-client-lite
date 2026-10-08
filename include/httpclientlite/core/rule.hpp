
#pragma once

/// @file rule.hpp
/// @brief Template-based rules for URI parameter extraction
///
/// Each rule describes how to extract one parameter from a parsed URI.
/// Rules are compile-time typed: the template parameter T specifies
/// the output type (std::string, std::vector<std::string>, or
/// std::unordered_map<std::string, std::string>).
///
/// Specializations:
///   rule<std::string, Loc>            — extract component as string
///   rule<vector<string>, Loc>         — split component by delimiter
///   rule<unordered_map<string,string>, location::query> — parse query as map

#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "components.hpp"

/// @namespace core
namespace core {

  /// @class param_value
  /// @brief Holds a parsed parameter value (string, list, or map)
  ///
  /// A lightweight variant type that stores the result of a rule extraction.
  /// The type is determined at construction time and accessed via typed methods.
  class param_value {
  public:
    /// @brief Type tag for runtime type identification
    enum class type_tag : std::uint8_t {
      empty,   ///< No value
      string,  ///< Single string value
      list,    ///< Ordered list of strings
      map      ///< Key-value pairs
    };

  private:
    type_tag tag_;
    std::string str_;
    std::vector<std::string> list_;
    std::unordered_map<std::string, std::string> map_;

  public:
    /// @brief Default constructor (empty value)
    param_value() noexcept
      : tag_(type_tag::empty) {}

    /// @brief Construct with string value
    explicit param_value(std::string value)
      : tag_(type_tag::string)
      , str_(std::move(value)) {}

    /// @brief Construct with list value
    explicit param_value(std::vector<std::string> values)
      : tag_(type_tag::list)
      , list_(std::move(values)) {}

    /// @brief Construct with map value
    explicit param_value(std::unordered_map<std::string, std::string> values)
      : tag_(type_tag::map)
      , map_(std::move(values)) {}

    param_value(param_value const&) = default;
    param_value(param_value&&) noexcept = default;
    param_value& operator=(param_value const&) = default;
    param_value& operator=(param_value&&) noexcept = default;
    ~param_value() = default;

    /// @brief Returns the type tag
    type_tag type() const noexcept {
      return tag_;
    }

    /// @brief Returns true if the value is empty (no value set)
    bool empty() const noexcept {
      switch (tag_) {
        case type_tag::empty:
          return true;
        case type_tag::string:
          return str_.empty();
        case type_tag::list:
          return list_.empty();
        case type_tag::map:
          return map_.empty();
      }
      return true;
    }

    /// @brief Get the string value (asserts type == string)
    const std::string& as_string() const {
      assert(tag_ == type_tag::string);
      return str_;
    }

    /// @brief Get the list value (asserts type == list)
    const std::vector<std::string>& as_list() const {
      assert(tag_ == type_tag::list);
      return list_;
    }

    /// @brief Get the map value (asserts type == map)
    const std::unordered_map<std::string, std::string>& as_map() const {
      assert(tag_ == type_tag::map);
      return map_;
    }
  };

  /// @class rule_base
  /// @brief Abstract base class for URI extraction rules
  ///
  /// All rules inherit from this interface. The extract() method
  /// takes a parsed components struct and returns a param_value.
  class rule_base {
  public:
    virtual ~rule_base() = default;

    rule_base() = default;
    rule_base(rule_base const&) = delete;
    rule_base& operator=(rule_base const&) = delete;
    rule_base(rule_base&&) = delete;
    rule_base& operator=(rule_base&&) = delete;

    /// @brief Returns the name of this rule
    [[nodiscard]] virtual std::string name() const = 0;

    /// @brief Extract a value from the given URI components
    [[nodiscard]] virtual param_value extract(const components& comp) const = 0;
  };

  /// @class rule
  /// @brief Template-based extraction rule (primary template — not defined)
  ///
  /// @tparam T - output type (std::string, std::vector<std::string>, etc.)
  /// @tparam Loc - which URI component to extract from
  template <typename T, location Loc>
  class rule;

  // =========================================================================
  // Specialization: rule<std::string, Loc> — extract component as string
  // =========================================================================

  /// @brief Extract a URI component as a single string
  ///
  /// Usage:
  /// @code
  ///   rule<std::string, location::scheme> r("protocol");
  ///   param_value val = r.extract(components);
  ///   // val.as_string() == "http"
  /// @endcode
  template <location Loc>
  class rule<std::string, Loc> : public rule_base {
  private:
    std::string name_;

  public:
    /// @param name - rule name for lookup
    explicit rule(std::string name)
      : name_(std::move(name)) {}

    [[nodiscard]] std::string name() const override {
      return name_;
    }

    [[nodiscard]] param_value extract(const components& comp) const override {
      string_chunk<> chunk;
      switch (Loc) {
        case location::scheme:
          chunk = comp.scheme;
          break;
        case location::userinfo:
          chunk = comp.userinfo;
          break;
        case location::host:
          chunk = comp.host;
          break;
        case location::port:
          chunk = comp.port;
          break;
        case location::path:
          chunk = comp.path;
          break;
        case location::query:
          chunk = comp.query;
          break;
        case location::fragment:
          chunk = comp.fragment;
          break;
        case location::authority:
          break;  // never stored as a whole
      }
      return param_value(chunk.str());
    }
  };

  // =========================================================================
  // Specialization: rule<std::vector<std::string>, Loc> — split by delimiter
  // =========================================================================

  /// @brief Split a URI component into a list of strings
  ///
  /// Usage:
  /// @code
  ///   rule<std::vector<std::string>, location::path> r("segments", '/');
  ///   param_value val = r.extract(components);
  ///   // val.as_list() == {"images", "logo.png"}
  /// @endcode
  template <location Loc>
  class rule<std::vector<std::string>, Loc> : public rule_base {
  private:
    std::string name_;
    char delimiter_;

  public:
    /// @param name - rule name for lookup
    /// @param delimiter - character to split on (default '/')
    explicit rule(std::string name, char delimiter = '/')
      : name_(std::move(name))
      , delimiter_(delimiter) {}

    [[nodiscard]] std::string name() const override {
      return name_;
    }

    [[nodiscard]] param_value extract(const components& comp) const override {
      string_chunk<> chunk;
      switch (Loc) {
        case location::scheme:
          chunk = comp.scheme;
          break;
        case location::userinfo:
          chunk = comp.userinfo;
          break;
        case location::host:
          chunk = comp.host;
          break;
        case location::port:
          chunk = comp.port;
          break;
        case location::path:
          chunk = comp.path;
          break;
        case location::query:
          chunk = comp.query;
          break;
        case location::fragment:
          chunk = comp.fragment;
          break;
        case location::authority:
          break;  // never stored as a whole
      }
      return param_value(detail::split(chunk, delimiter_));
    }
  };

  // =========================================================================
  // Specialization: rule<map, location::query> — parse query string as map
  // =========================================================================

  /// @brief Parse a query string into a key-value map
  ///
  /// Usage:
  /// @code
  ///   rule<std::unordered_map<std::string, std::string>, location::query> r("params");
  ///   param_value val = r.extract(components);
  ///   // val.as_map() == {"q":"laptop", "page":"2"}
  /// @endcode
  template <>
  class rule<std::unordered_map<std::string, std::string>, location::query> : public rule_base {
  private:
    std::string name_;
    char kv_delimiter_;
    char pair_delimiter_;

  public:
    /// @param name - rule name for lookup
    /// @param kv_delim - key-value delimiter (default '=')
    /// @param pair_delim - pair delimiter (default '&')
    explicit rule(std::string name, char kv_delim = '=', char pair_delim = '&')
      : name_(std::move(name))
      , kv_delimiter_(kv_delim)
      , pair_delimiter_(pair_delim) {}

    [[nodiscard]] std::string name() const override {
      return name_;
    }

    [[nodiscard]] param_value extract(const components& comp) const override {
      return param_value(detail::parse_query(comp.query, kv_delimiter_, pair_delimiter_));
    }
  };

  // =========================================================================
  // Factory functions for convenient rule creation
  // =========================================================================

  /// @brief Create a string extraction rule
  template <location Loc>
  inline std::unique_ptr<rule_base> make_string_rule(const std::string& name) {
    return std::make_unique<rule<std::string, Loc>>(name);
  }

  /// @brief Create a list extraction rule (split by delimiter)
  template <location Loc>
  inline std::unique_ptr<rule_base> make_list_rule(const std::string& name, char delimiter = '/') {
    return std::make_unique<rule<std::vector<std::string>, Loc>>(name, delimiter);
  }

  /// @brief Create a map extraction rule (parse query string)
  inline std::unique_ptr<rule_base> make_map_rule(const std::string& name, char kv_delim = '=',
                                                  char pair_delim = '&') {
    return std::make_unique<rule<std::unordered_map<std::string, std::string>, location::query>>(
    name, kv_delim, pair_delim);
  }

}  // namespace core
