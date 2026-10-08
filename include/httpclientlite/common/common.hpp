
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

#include "type_traits.hpp"

#if __cplusplus >= 201703L  // C++17: use std::string_view internally
#  include <string_view>
#endif

/// @namespace common
namespace common {

#if __cplusplus >= 201703L  // =====================================================
  // C++17 implementation: string_chunk wraps std::basic_string_view
  // ==================================================================================

  /// @class string_chunk
  /// @brief Non-owning view over a contiguous character buffer
  ///
  /// A thin wrapper around std::basic_string_view that preserves the project-specific
  /// API (str(), substr() with empty-on-OOB, at() with assert) while using the
  /// standard string_view as the underlying implementation when C++17 is available.
  /// The pointed-to memory must outlive the string_chunk instance.
  /// @tparam CHAR_TYPE - Internal data type, must be a character type
  template <typename CHAR_TYPE = char>
  class string_chunk final {
    static_assert(common::is_char_type<CHAR_TYPE>::value, "Wrong char type");

    using view_type = std::basic_string_view<CHAR_TYPE>;
    view_type view_;

  public:
    /// @brief Type alias for the element type
    using value_type = CHAR_TYPE;

    /// @brief Type alias for mutable pointer (kept for backward compatibility)
    using pointer = CHAR_TYPE*;
    /// @brief Type alias for const pointer
    using const_pointer = CHAR_TYPE const*;
    /// @brief Type alias for mutable reference (kept for backward compatibility)
    using reference = CHAR_TYPE&;
    /// @brief Type alias for const reference
    using const_reference = CHAR_TYPE const&;

    /// @brief Iterator type (const, since string_chunk is non-owning and read-only)
    using iterator = const_pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    /// @brief Constant for "not found" position, same as string_view::npos
    static constexpr size_type npos = view_type::npos;

    string_chunk() = default;
    ~string_chunk() = default;

    /// @brief Construct from raw pointer and size
    /// @param ptr - pointer to character data
    /// @param sz - number of characters
    // cppcheck-suppress passedByValue  -- size_t is cheap to copy
    explicit string_chunk(CHAR_TYPE const* ptr, size_type sz) noexcept
      : view_(ptr, sz) {}

    /// @brief Construct from std::basic_string reference
    /// @param str - source string
    explicit string_chunk(std::basic_string<CHAR_TYPE> const& str) noexcept
      : view_(str) {}

    /// @brief Construct from another string_chunk (implicit, like string_view)
    string_chunk(string_chunk const&) noexcept = default;
    string_chunk(string_chunk&&) noexcept = default;

    string_chunk& operator=(string_chunk const&) noexcept = default;
    string_chunk& operator=(string_chunk&&) noexcept = default;

    /// @brief Access element by index (no bounds checking)
    // cppcheck-suppress passedByValue  -- size_t is cheap to copy
    const_reference operator[](size_type pos) const {
      return view_[pos];
    }

    /// @brief Equality comparison (content-based, like string_view)
    bool operator==(string_chunk const& other) const noexcept {
      return view_ == other.view_;
    }

    /// @brief Inequality comparison
    bool operator!=(string_chunk const& other) const noexcept {
      return view_ != other.view_;
    }

    [[nodiscard]] const_iterator begin() const noexcept {
      return view_.data();
    }

    [[nodiscard]] const_iterator end() const noexcept {
      return view_.data() + view_.size();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
      return begin();
    }

    [[nodiscard]] const_iterator cend() const noexcept {
      return end();
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
      return const_reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept {
      return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
      return rbegin();
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept {
      return rend();
    }

    /// @brief Returns true if the view is empty (size == 0)
    [[nodiscard]] bool empty() const noexcept {
      return view_.empty();
    }

    /// @brief Returns the number of characters in the view
    [[nodiscard]] size_type size() const noexcept {
      return view_.size();
    }

    /// @brief Returns a pointer to the underlying character data
    [[nodiscard]] const_pointer data() const noexcept {
      return view_.data();
    }

    /// @brief Searches for the first occurrence of a substring
    /// @param str - C-string to search for (no allocation)
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    size_type find(CHAR_TYPE const* str, size_type pos = 0) const noexcept {
      return view_.find(str, pos);
    }

    /// @brief Searches for the first occurrence of a single character
    /// @param c - character to search for
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    [[nodiscard]] size_type find(CHAR_TYPE c, size_type pos = 0) const noexcept {
      return view_.find(c, pos);
    }

    /// @brief Searches for the first occurrence of a substring
    /// @param str - std::basic_string to search for
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    [[nodiscard]] size_type find(std::basic_string<CHAR_TYPE> const& str,
                                 size_type pos = 0) const noexcept {
      return view_.find(view_type(str), pos);
    }

    /// @brief Returns a std::basic_string copy of the view (project-specific, not in string_view)
    [[nodiscard]] std::basic_string<CHAR_TYPE> str() const {
      return view_.empty() ? std::basic_string<CHAR_TYPE>() : std::basic_string<CHAR_TYPE>(view_);
    }

    /// @brief Access element by index with bounds assertion (not throw like string_view)
    // cppcheck-suppress passedByValue  -- size_t is cheap to copy
    [[nodiscard]] const_reference at(size_type pos) const {
      assert(pos < view_.size());
      return view_[pos];
    }

    /// @brief Returns a sub-view starting at pos with given count
    ///
    /// Unlike std::string_view::substr which throws std::out_of_range on OOB,
    /// this method returns an empty chunk for backward compatibility.
    /// @param pos - start position
    /// @param count - number of characters (default: npos = to the end)
    /// @return A new string_chunk for the sub-range, or empty chunk if pos is out of bounds
    // cppcheck-suppress passedByValue  -- size_t is cheap to copy
    [[nodiscard]] string_chunk substr(size_type pos, size_type count = npos) const noexcept {
      if (pos >= view_.size()) {
        return string_chunk();
      }

      size_type effective_count = (count > view_.size() - pos) ? (view_.size() - pos) : count;

      return string_chunk(view_.data() + pos, effective_count);
    }
  };

#else  // C++11 / C++14 ======================================================
  // Custom implementation (no std::string_view available)
  // ==================================================================================

  /// @class string_chunk
  /// @brief Non-owning view over a contiguous character buffer
  ///
  /// A lightweight string_view-like class that stores a pointer and size without owning the data.
  /// The pointed-to memory must outlive the string_chunk instance.
  /// @tparam CHAR_TYPE - Internal data type, must be a character type
  template <typename CHAR_TYPE = char>
  class string_chunk final {
    static_assert(common::is_char_type<CHAR_TYPE>::value, "Wrong char type");

  public:
    /// @brief Type alias for the element type
    typedef CHAR_TYPE value_type;

    /// @brief Type alias for mutable pointer
    typedef CHAR_TYPE* pointer;
    /// @brief Type alias for const pointer
    typedef CHAR_TYPE const* const_pointer;
    /// @brief Type alias for mutable reference
    typedef CHAR_TYPE& reference;
    /// @brief Type alias for const reference
    typedef CHAR_TYPE const& const_reference;

    /// @brief Iterator type (const, since string_chunk is non-owning and read-only)
    using iterator = const_pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    /// @brief Constant for "not found" position, same as std::string::npos
    static constexpr size_type npos = std::string::npos;

  private:
    const_pointer data_ = nullptr;
    size_type size_ = 0;

  public:
    string_chunk() = default;
    string_chunk(string_chunk const& sw) noexcept
      : data_(sw.data_)
      , size_(sw.size_) {}

    /// @brief Construct from raw pointer and size
    /// @param ptr - pointer to character data
    /// @param sz - number of characters
    explicit string_chunk(char const* ptr, size_t sz) noexcept
      : data_(ptr)
      , size_(sz) {}

    /// @brief Construct from std::string reference
    /// @param str - source string
    explicit string_chunk(std::string const& str) noexcept
      : data_(str.data())
      , size_(str.size()) {}

    /// @brief Access element by index (no bounds checking)
    const_reference operator[](size_type pos) const {
      return data_[pos];
    }

    string_chunk& operator=(string_chunk const& other) noexcept {
      data_ = other.data_;
      size_ = other.size_;

      return *this;
    }

    /// @brief Equality comparison (content-based)
    bool operator==(string_chunk const& other) const noexcept {
      return (size_ == other.size_) &&
             (std::char_traits<CHAR_TYPE>::compare(data_, other.data_, size_) == 0);
    }

    /// @brief Inequality comparison
    bool operator!=(string_chunk const& other) const noexcept {
      return !(*this == other);
    }

    const_iterator begin() const noexcept {
      return data_;
    }

    const_iterator end() const noexcept {
      return data_ + size_;
    }

    const_iterator cbegin() const noexcept {
      return begin();
    }

    const_iterator cend() const noexcept {
      return end();
    }

    const_reverse_iterator rbegin() const noexcept {
      return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept {
      return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
      return rbegin();
    }

    const_reverse_iterator crend() const noexcept {
      return rend();
    }

    /// @brief Returns true if the view is empty (size == 0)
    bool empty() const noexcept {
      return (size_ == 0);
    }

    /// @brief Returns the number of characters in the view
    size_type size() const noexcept {
      return size_;
    }

    /// @brief Returns a pointer to the underlying character data
    const_pointer data() const noexcept {
      return data_;
    }

    /// @brief Searches for the first occurrence of a substring
    /// @param str - C-string to search for (no allocation)
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    size_type find(char const* str, size_type pos = 0) const noexcept {
      assert(str != nullptr);

      size_type len = std::char_traits<char>::length(str);

      return (pos >= size_ || len > size_ - pos)
             ? npos
             : to_pos(std::search(data_ + pos, data_ + size_, str, str + len));
    }

    /// @brief Searches for the first occurrence of a single character
    /// @param c - character to search for
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    size_type find(char c, size_type pos = 0) const noexcept {
      if (pos >= size_) {
        return npos;
      }

      char const* found = std::char_traits<char>::find(data_ + pos, size_ - pos, c);
      return (found != nullptr) ? static_cast<size_type>(found - data_) : npos;
    }

    /// @brief Searches for the first occurrence of a substring
    /// @param str - std::string to search for
    /// @param pos - position to start searching from
    /// @return Position of the first match, or npos if not found
    size_type find(std::string const& str, size_type pos = 0) const noexcept {
      assert(str.data() != nullptr || str.size() == 0);

      return (pos >= size_)
             ? npos
             : to_pos(std::search(data_ + pos, data_ + size_, str.begin(), str.end()));
    }

    /// @brief Returns a std::string copy of the view
    std::string str() const noexcept {
      return (empty()) ? std::string() : std::string(data_, size_);
    }

    /// @brief Access element by index with bounds assertion
    const_reference at(size_type pos) const {
      assert(pos < size());
      return data_[pos];
    }

    /// @brief Returns a sub-view starting at pos with given count
    /// @param pos - start position
    /// @param count - number of characters (default: npos = to the end)
    /// @return A new string_chunk for the sub-range, or empty chunk if pos is out of bounds
    string_chunk substr(size_type pos, size_type count = npos) const noexcept {
      if (pos >= size_) {
        return string_chunk();
      }

      size_type effective_count = (count > size_ - pos) ? (size_ - pos) : count;

      return string_chunk(data_ + pos, effective_count);
    }

  protected:
    size_type to_pos(const_reverse_iterator it) const {
      return (it == crend()) ? npos : size_type(crend() - it - 1);
    }

    size_type to_pos(const_iterator it) const {
      return (it == cend()) ? npos : size_type(it - cbegin());
    }
  };

#endif /* Check C++ Standard */

}  // namespace common
