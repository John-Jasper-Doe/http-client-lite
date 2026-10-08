
#pragma once

/// @file grammar.hpp
/// @brief Static URI parsing rules, known at compile time
///
/// The tables below declare how a URI string is decomposed into
/// components (RFC3986). They are fixed and known in advance —
/// parser.hpp executes them; nothing is registered at runtime.
///
/// Top level:    scheme ":" ["//"] authority path ["?" query] ["#" fragment]
/// Authority:    [userinfo "@"] host [":" port]
///
/// Each step describes one region: an optional literal prefix that gates
/// the step, a set of terminator characters and what to do when a
/// terminator is missing. Steps with `sub` are groups: the region
/// delimited by `stop` is processed recursively by the nested rules.

#include <algorithm>
#include <cstddef>
#include <string_view>

#include "components.hpp"

/// @namespace core
namespace core {

  /// @namespace core::grammar
  /// @brief Declarative parsing rules and the engine that executes them
  namespace grammar {

    /// @brief Behavior when a step's terminator is not found
    enum class on_miss {
      to_end,            ///< component spans to the end of the region
      skip,              ///< component stays empty, position unchanged
      terminate_as_path  ///< no scheme: whole URI is a path, stop parsing
    };

    /// @struct step
    /// @brief Static descriptor of one parsing step
    struct step {
      location loc;             ///< component to fill (ignored for groups)
      std::string_view stop;    ///< terminator characters ("" = to region end)
      std::string_view prefix;  ///< literal gate before the step ("" = ungated)
      on_miss miss;             ///< behavior when `stop` is not found
      bool consume_prefix;      ///< advance past `prefix` when it matches
      bool consume_stop;        ///< advance past the terminator when found
      bool bracketed;           ///< host: leading "[...]" (IPv6) instead of ':'-split
      const step* sub;          ///< group: nested steps over the delimited region
      std::size_t sub_size;     ///< number of nested steps
    };

    /// Authority: [userinfo "@"] host [":" port]
    inline constexpr step authority_steps[] = {
      // loc              stop   prefix  miss                  cprefix cstop  brack  sub  n
      { location::userinfo, "@", "", on_miss::skip, false, true, false, nullptr, 0 },
      { location::host, ":", "", on_miss::to_end, false, false, true, nullptr, 0 },
      { location::port, "", ":", on_miss::skip, true, false, false, nullptr, 0 },
    };

    /// scheme ":" ["//"] authority path ["?" query] ["#" fragment]
    inline constexpr step uri_steps[] = {
      // loc              stop    prefix  miss                       cprefix cstop  brack  sub n
      { location::scheme, ":", "", on_miss::terminate_as_path, false, true, false, nullptr, 0 },
      { location::authority, "/?#", "//", on_miss::to_end, true, false, false, authority_steps, 3 },
      { location::path, "?#", "", on_miss::to_end, false, false, false, nullptr, 0 },
      { location::query, "#", "?", on_miss::to_end, true, false, false, nullptr, 0 },
      { location::fragment, "", "#", on_miss::to_end, true, false, false, nullptr, 0 },
    };

    // ---------------------------------------------------------------------
    // Engine
    // ---------------------------------------------------------------------

    inline constexpr std::size_t npos = static_cast<std::size_t>(-1);

    /// @brief First occurrence of any of `chars` in [pos, bound)
    inline std::size_t find_any(const char* data, std::size_t pos, std::size_t bound,
                                std::string_view chars) {
      if (bound <= pos) {
        return npos;
      }
      auto const it = std::find_if(data + pos, data + bound, [&chars](char c) {
        return chars.find(c) != std::string_view::npos;
      });
      return it == data + bound ? npos : static_cast<std::size_t>(it - data);
    }

    /// @brief True if `p` matches `data` at `pos` within [pos, bound)
    inline bool match_prefix(const char* data, std::size_t pos, std::size_t bound,
                             std::string_view p) {
      if (p.empty()) {
        return true;
      }
      if (pos + p.size() > bound) {
        return false;
      }
      for (std::size_t i = 0; i < p.size(); ++i) {
        if (data[pos + i] != p[i]) {
          return false;
        }
      }
      return true;
    }

    /// @brief Store [off, off + len) into the component named by `loc`
    inline void assign(components& out, location loc, const char* data, std::size_t off,
                       std::size_t len) {
      if (len == 0) {
        return;
      }
      string_chunk<> chunk(data + off, len);
      switch (loc) {
        case location::scheme:
          out.scheme = chunk;
          break;
        case location::userinfo:
          out.userinfo = chunk;
          break;
        case location::host:
          out.host = chunk;
          break;
        case location::port:
          out.port = chunk;
          break;
        case location::path:
          out.path = chunk;
          break;
        case location::query:
          out.query = chunk;
          break;
        case location::fragment:
          out.fragment = chunk;
          break;
        case location::authority:
          break;  // region only — split into userinfo/host/port
      }
    }

    /// @brief Execute a sequence of steps over [pos, bound)
    /// @return false if parsing was terminated early (terminate_as_path)
    inline bool run_steps(const step* steps, std::size_t count, const char* data, std::size_t& pos,
                          std::size_t bound, components& out) {
      for (std::size_t i = 0; i < count; ++i) {
        const step& s = steps[i];

        // Literal gate: mismatched prefix skips the step entirely
        if (!s.prefix.empty()) {
          if (!match_prefix(data, pos, bound, s.prefix)) {
            continue;
          }
          if (s.consume_prefix) {
            pos += s.prefix.size();
          }
        }

        // Group: delimit the region and process nested rules over it
        if (s.sub != nullptr) {
          std::size_t e = find_any(data, pos, bound, s.stop);
          if (e == npos) {
            e = bound;
          }
          std::size_t sub_pos = pos;
          if (!run_steps(s.sub, s.sub_size, data, sub_pos, e, out)) {
            return false;
          }
          pos = e;  // terminator not consumed — the next step sees it
          continue;
        }

        // Bracketed host: "[...]" — "]" terminates, ':' after it is the port
        if (s.bracketed && pos < bound && data[pos] == '[') {
          std::size_t br = find_any(data, pos, bound, "]");
          if (br == npos) {
            pos = bound;  // unterminated — host and port stay empty
            continue;
          }
          assign(out, s.loc, data, pos + 1, br - (pos + 1));
          pos = br + 1;
          continue;
        }

        // Plain step: read until a terminator
        if (s.stop.empty()) {
          assign(out, s.loc, data, pos, bound - pos);
          pos = bound;
          continue;
        }

        std::size_t e = find_any(data, pos, bound, s.stop);
        if (e == npos) {
          switch (s.miss) {
            case on_miss::to_end:
              assign(out, s.loc, data, pos, bound - pos);
              pos = bound;
              break;
            case on_miss::skip:
              break;  // empty component, position unchanged
            case on_miss::terminate_as_path:
              assign(out, location::path, data, pos, bound - pos);
              return false;
          }
          continue;
        }

        assign(out, s.loc, data, pos, e - pos);
        if (s.consume_stop) {
          pos = e + 1;
        }
        else {
          pos = e;
        }
      }
      return true;
    }

    /// @brief Parse a URI according to the static rule tables
    /// @param uri - the URI string (must outlive the views stored in `out`)
    /// @param out - components to fill
    inline void parse(std::string_view uri, components& out) {
      const char* data = uri.data();
      std::size_t pos = 0;
      std::size_t bound = uri.size();
      run_steps(uri_steps, sizeof(uri_steps) / sizeof(uri_steps[0]), data, pos, bound, out);
    }

  }  // namespace grammar

}  // namespace core
