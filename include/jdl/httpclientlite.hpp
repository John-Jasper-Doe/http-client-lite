/*
 *   httpclientlite.hpp
 *   ===========================================================================================
 *
 *   The MIT License
 *
 *   Copyright (c) 2016 Christian C. Sachs
 *   Copyright (c) 2021 Maxim G.
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#pragma once

#if defined(__linux__)
#  define PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#  define PLATFORM_WINDOWS
#else
/* TODO:
 *  - Added Apple OS */

/* warning: Unknown OS */
#endif

#include <sys/types.h>

#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>

#if defined(PLATFORM_WINDOWS)
#  include <WS2tcpip.h>
#  include <WinSock2.h>

typedef SOCKET socktype_t;
typedef int    socklen_t;

#  pragma comment(lib, "ws2_32.lib")

#elif defined(PLATFORM_LINUX)
#  include <netdb.h>
#  include <sys/socket.h>
#  include <unistd.h>

#  define INVALID_SOCKET      -1
#  define closesocket(__sock) close(__sock)

typedef int socktype_t;

#endif /* PLATFORM_WINDOWS or PLATFORM_LINUX */


#if __cplusplus

/// Macro for constexpr, to support in mixed 03/0x mode.
#ifndef JDLCXX_CONSTEXPR
# if __cplusplus >= 201103L
#  define JDLCXX_CONSTEXPR constexpr
#  define JDLCXX_USE_CONSTEXPR constexpr
# else
#  define JDLCXX_CONSTEXPR
#  define JDLCXX_USE_CONSTEXPR const
# endif
#endif

#ifndef JDLCXX14_CONSTEXPR
# if __cplusplus >= 201402L
#  define JDLCXX14_CONSTEXPR constexpr
# else
#  define JDLCXX14_CONSTEXPR
# endif
#endif

#ifndef JDLCXX17_CONSTEXPR
# if __cplusplus >= 201703L
#  define JDLCXX17_CONSTEXPR constexpr
# else
#  define JDLCXX17_CONSTEXPR
# endif
#endif

#ifndef JDLCXX20_CONSTEXPR
# if __cplusplus > 201703L
#  define JDLCXX20_CONSTEXPR constexpr
# else
#  define JDLCXX20_CONSTEXPR
# endif
#endif

#ifndef JDLCXX17_INLINE
# if __cplusplus >= 201703L
#  define JDLCXX17_INLINE inline
# else
#  define JDLCXX17_INLINE
# endif
#endif

#endif /* __cplusplus */






const std::string content_type { "Content-Type: text/plain; version=0.0.4; charset=utf-8" };

const std::string http_new_line { "\r\n" };
const std::string http_space { " " };
const std::string http_header_separator { ": " };


JDL_NAMESPACE_BEGIN

void init_socket() {
#if defined(PLATFORM_WINDOWS)
  WSADATA wsa_data;
  WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif /* PLATFORM_WINDOWS */
}

void deinit_socket() {
#if defined(PLATFORM_WINDOWS)
  WSACleanup();
#endif /* PLATFORM_WINDOWS */
}

// Default data on this module
using data_t = uint8_t;

#if __cplusplus >= 201703L  // If C++ Standart == 17

template <typename TYPE>
using is_integral_v = std::is_integral_v<TYPE>;

template <bool COND, typename TYPE = void>
using enable_if_t = typename std::enable_if_t<COND, TYPE>;

template<typename TYPE, typename COMPARED_TYPE >
JDLCXX17_INLINE constexpr bool is_same_v = std::is_same_v<TYPE, COMPARED_TYPE>;

template <typename TYPE>
using remove_cv_t = typename std::remove_cv_t<TYPE>;

#elif __cplusplus >= 201402L  // If C++ Standart == 14

template <typename TYPE>
using is_integral_v = std::is_integral_v<TYPE>;

template <bool COND, typename TYPE = void>
using enable_if_t = typename std::enable_if_t<COND, TYPE>;

template<typename TYPE, typename COMPARED_TYPE >
JDLCXX17_INLINE constexpr bool is_same_v = std::is_same<TYPE, COMPARED_TYPE>::value;

template <typename TYPE>
using remove_cv_t = typename std::remove_cv_t<TYPE>;

#else  // If C++ Standart == 11

// Trates
template <typename TYPE>
JDLCXX17_INLINE constexpr bool is_integral_v = std::is_integral<TYPE>::value;

template <bool COND, typename TYPE = void>
using enable_if_t = typename std::enable_if<COND, TYPE>::type;

template<typename TYPE, typename COMPARED_TYPE >
JDLCXX17_INLINE constexpr bool is_same_v = std::is_same<TYPE, COMPARED_TYPE>::value;

template <typename TYPE>
using remove_cv_t = typename std::remove_cv<TYPE>::type;

template <typename TYPE>
using remove_pointer_t = typename std::remove_pointer<TYPE>::type;

#endif /* Check C++ Standart */


/// is_require_type_v
/// @brief Comparing the received type with the required type
///
/// The resulting type is reduced to a primitive, i.e. "const" and "volatile" are removed
/// and this naked type is compared with the specified type.
/// @tparam CHECK_TYPE - resulting type
/// @tparam TYPE - required type
template <typename CHECK_TYPE, typename TYPE>
constexpr bool is_require_type_v = is_same_v<remove_cv_t<remove_pointer_t<CHECK_TYPE>>, TYPE>;


/// is_char_type of false_type
/// @brief Checking whether the type is a character type
template <typename, typename CHECK = void>
struct is_char_type : std::false_type { };

/// is_char_type of true_type
/// @brief Checking whether the type is a character type
template <typename TYPE>
struct is_char_type<TYPE, enable_if_t<is_require_type_v<TYPE, char       > ||
                                      is_require_type_v<TYPE, wchar_t    > ||
                                      is_require_type_v<TYPE, char16_t   > ||
                                      is_require_type_v<TYPE, char32_t   >
    >> : std::true_type { };

/// is_char_type_v
/// @brief Aliase of is_char_type
template <typename TYPE>
constexpr bool is_char_type_v = is_char_type<TYPE>::value;









  class tokenizer {
    const std::string str_;
    std::size_t       position_;

   public:
    explicit tokenizer(const std::string& str) : str_(std::move(str)), position_(0) {}

    std::string next(const std::string& search, bool return_tail = false) {
      std::size_t hit = str_.find(search, position_);
      if (hit == std::string::npos) {
        if (return_tail) {
          return tail();
        } else {
          return std::string();
        }
      }

      std::size_t old_position = position_;
      position_                = hit + search.length();

      return str_.substr(old_position, hit - old_position);
    }

    std::string tail() {
      size_t old_position = position_;
      position_           = str_.length();

      return str_.substr(old_position);
    }
  };

  using string_map_t = std::unordered_map<std::string, std::string>;

















/// @namespace common
/// @brief Description of the general functionality for this module
namespace common {

  /// @class string_chunk
  /// @brief Implementation of the string_view micro class
  ///
  /// A small self-written implementation of the class resembling "string_view" for
  /// ease of operation.
  /// @tparam CHAR_TYPE - Internal data type
  template <typename CHAR_TYPE = char>
  class string_chunk final {
    static_assert (jdl::is_char_type_v<CHAR_TYPE>, "Wrong char type");

  public:
    using value_type             = CHAR_TYPE;

    using pointer                = CHAR_TYPE*;
    using const_pointer          = CHAR_TYPE const*;
    using reference              = CHAR_TYPE&;
    using const_reference        = CHAR_TYPE const&;

    using iterator               = const_pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;

  private:
    const_pointer data_ = nullptr;
    size_type     size_ = 0;

  public:
    string_chunk() = default;
    string_chunk(string_chunk const& sw)     noexcept : data_(sw.data_), size_(sw.size_) { }
    string_chunk(char const* ptr, size_t sz) noexcept : data_(ptr), size_(sz) { }
    string_chunk(std::string const& str)     noexcept : data_(str.data()), size_(str.size()) { }

    const_reference operator[](size_type pos) const { return data_[pos]; }

    string_chunk& operator=(string_chunk const& other) noexcept {
      data_ = other.data_;
      size_ = other.size_;

      return *this;
    }

    bool operator==(string_chunk const& other) const noexcept {
      return ((data_ == other.data_) && (size_ == other.size_) );
    }

    const_iterator         begin()   const noexcept { return data_;         }
    const_iterator         end()     const noexcept { return data_ + size_; }

    const_iterator         cbegin()  const noexcept { return begin(); }
    const_iterator         cend()    const noexcept { return end();   }

    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end());   }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend()   const noexcept { return rend();   }

    bool                   empty()   const noexcept { return (size_ == 0); }
    size_type              size()    const noexcept { return size_; }
    const_pointer          data()    const noexcept { return data_; }

    size_type find(std::string const& str, size_t pos = 0) {
      assert(str.data() != nullptr || str.size() == 0);

      return (pos >= size_)
          ? std::string::npos
          : to_pos(std::search(data_ + pos, data_ + size_, str.begin(), str.end()));
    }

    std::string str() const noexcept {
      return (empty()) ? std::string() : std::string(data_, size_);
    }

    const_reference at(size_type pos) const {
      assert (pos < size());
      return data_[pos];
    }

  protected:
    size_type to_pos(const_reverse_iterator it) const {
      return (it == crend()) ? std::string::npos : size_type(crend() - it - 1);
    }

    size_type to_pos(const_iterator it) const {
      return (it == cend()) ? std::string::npos : size_type(it - cbegin());
    }
  };


  /// @struct offeter
  /// @brief The storage struct of the offset in the buffer and the size of the data
  ///
  /// The structure of storing the offset in a given buffer and the size of the data. It helps
  /// to store data economically without copying. This structure must be inherited in order to
  /// register the data in which the offsets must be stored.
  /// @tparam OFFSET_TYPE - offset type, by default "uint16_t"
  template <typename OFFSET_TYPE = uint16_t>
  struct offseter {
    static_assert (jdl::is_integral_v<OFFSET_TYPE>, "Wrong type, should be integral");

    using offset_type        = OFFSET_TYPE;
    using size_type          = size_t;
    using internel_size_type = OFFSET_TYPE;

  private:
    OFFSET_TYPE offset_ = 0;
    OFFSET_TYPE size_   = 0;

  public:
    offseter() = default;
    offseter(offset_type offset, offset_type size) noexcept
      : offset_(offset), size_(size)
    { }

    bool        is_empty() const noexcept { return (offset_ == 0 && size_ == 0); }

    size_type   size()     const noexcept { return static_cast<size_type>(size_); }
    offset_type offset()   const noexcept { return offset_; }

    // !!! The GET function should be written in the heirs !!!

    void set(offset_type offset, offset_type size) noexcept {
      offset_ = offset;
      size_   = size;
    }

    bool operator==(offseter<OFFSET_TYPE> const other) const noexcept {
      return (offset_ == other.offset_ && size_ == other.size_);
    }

    bool operator!=(offseter<OFFSET_TYPE> const other) const noexcept {
      return (offset_ != other.offset_ && size_ != other.size_);
    }

    friend std::ostream& operator<<(std::ostream&, offseter<> const&);
  };


  /// @fn operator<<
  /// @brief The data output operator of the "offseter" class
  ///
  /// The "offseter" class data output operator to the output stream, for checking and debugging.
  /// @param os - output stream
  /// @param of - offseter
  std::ostream& operator<<(std::ostream& os, offseter<> const& of) {
      return (os << "[ offset: " << of.offset_ << ", size: " << of.size_);
  }

} /* common:: */


/// @namespace details_http_client_lite
/// @brief Internal functionality of the module
namespace details_http_client_lite {

  /// @typedef cond_set_t
  /// @brief Conditional sets type
  ///
  /// The type is used to specify sets of characters for checking parts of the URI.
  using cond_set_t = std::unordered_set<char>;


  /// @typedef buffer_t
  /// @brief Buffer containing URI
  using buffer_t = std::string;


  /// @typedef const_buff_t
  /// @brief Constant buffer
  using const_buffer_t = buffer_t const;


  /// string_chunk_t
  /// @brief Aliase of string_chunk with char type
  using string_chunk_t = common::string_chunk<char>;


  /// @struct buff_offset
  /// @brief The heir of the @a"offseter" class for the buffer
  struct buff_offset final : public common::offseter<uint16_t> {
    buff_offset() = default;
    explicit buff_offset(buff_offset::offset_type offset, buff_offset::offset_type size) noexcept
      : common::offseter<uint16_t>(offset, size)
    { }

    string_chunk_t get(const_buffer_t& buff) const noexcept {
      return (buff.size() > (this->offset() + this->size()))
          ? string_chunk_t(buff.data() + this->offset(), this->size())
          : string_chunk_t();
    }
  };


  /// @struct checker
  /// @brief Checks for the specified set
  ///
  /// The controller checks the symbols in the set package.
  /// @tparam CHECK_PACK - the set package
  /// @return TRUE if the symbol is in at least one set, otherwise FALSE
  template <typename ...CHECK_PACK>
  struct checker final {
    using check_pack_t = std::tuple<CHECK_PACK...>;

    template <std::size_t IDX>
    static inline jdl::enable_if_t<IDX == sizeof...(CHECK_PACK), bool>
    process(char const&, check_pack_t const&) {
      return false;
    }

    template <std::size_t IDX>
    static inline jdl::enable_if_t<IDX < sizeof...(CHECK_PACK), bool>
    process(char const& ch, check_pack_t const& check_pack) {
      return (std::get<IDX>(check_pack).check(ch)) ? true : process<IDX + 1>(ch, check_pack);
    }

    bool operator()(string_chunk_t const& str) {
      check_pack_t check_pack;

      for (char const& ch : str) {
        if (!process<0>(ch, check_pack)) {
          return false;
        }
      }

      return true;
    }
  };


  /// checker_default_t
  /// @brief Aliase of checker by default without chack_pack
  using checker_default_t = checker<>;


  /// @class verify_cond
  /// @brief Data required for verification
  ///
  /// The class contains a set of characters to check for validity. The class is inherited
  /// and the data is filled in by the inheritors.
  class verify_cond {
    cond_set_t const cond_set_;

  public:
    verify_cond() = delete;
    explicit verify_cond(cond_set_t const& cond_set) noexcept
      : cond_set_(cond_set)
    { }

    bool check(char const& ch) const {
      return cond_set_.find(ch) != cond_set_.end();
    }
  };


  /// @class uri_base
  /// @brief URI base class
  ///
  /// The URI base class stores a buffer with the address and the necessary functionality
  /// for obtaining tokens.
  class uri_base {
    const_buffer_t& buff_ref_;

  public:
    uri_base() = delete;
    explicit uri_base(buffer_t& buffer) noexcept
      : buff_ref_(buffer)
    { }

    template <std::size_t POSITION = 0, int COUNT = -1, typename CHECKER = checker_default_t>
    buff_offset get_token(std::string const& delim) {
      string_chunk_t cnk(buff_ref_.data() + POSITION,
                         ((COUNT < 0) ? buff_ref_.size() : (size_t)COUNT));

      std::size_t find_pos = cnk.find(delim);
      if (find_pos != 0 && find_pos != std::string::npos &&
          CHECKER()(string_chunk_t(buff_ref_.data() + POSITION, find_pos - POSITION))) {
        return buff_offset(static_cast<buff_offset::offset_type>(POSITION),
                           static_cast<buff_offset::offset_type>(find_pos - POSITION));
      }

      return buff_offset();
    }

    const_buffer_t& buff_ref() const {
      return buff_ref_;
    }
  };















//    //DEPRICATE!!!!!
//    template <size_t START_POSITION>
//    inline std::string get_token(const std::string& str, const std::string& delim, checker_t<> checker) {
//      std::size_t hit = str.find(delim, START_POSITION);
//      if (hit != std::string::npos) {
//        if (checker(str.substr(START_POSITION, hit - START_POSITION)))
//          return str.substr(START_POSITION, hit - START_POSITION);
//      }

//      return std::string();
//    }


































//    struct authority {
//      struct userinfo {
//        offseter<> username;
//        offseter<> password;
//      };

//      struct host {
//        offseter<> hostname;
//        offseter<> port;
//      };

//      authority(buffer<>& /*buffer*/){}
//    };


//    struct query {
//      std::unordered_map<offseter<>, offseter<>> store;
//    };


//    using schem_t     = offseter<>;
//    using authority_t = authority;
//    using path_t      = offseter<>;
//    using query_t     = query;
//    using fragment_t  = offseter<>;


//    class url {
//      schem_t     schem_;
////      authority_t authority_;
//      path_t      path_;
////      query_t     query_;
//      fragment_t  fragment_;

//    public:
//      url() = delete;
//      explicit url(buffer<>& buffer)
//        : schem_(buffer.template get_token<>("://"))
////        , authority_()
//        , path_()
////        , query_(std::make_pair<offseter<>, offseter<>>(offseter<>(), offseter<>()))
//        , fragment_()
//      { }
//    };


}  /* details_http_client_lite:: */


/// @namespace uri
namespace uri {

  /// @struct verify_cond_gen_delims
  /// @brief gen_delims - they are also "main delimiters"
  ///
  /// This characters that divide URIs into large components.
  struct verify_cond_gen_delims final : public details_http_client_lite::verify_cond {
    verify_cond_gen_delims() noexcept
      : verify_cond({ ':', '/', '?', '#', '[', ']', '@' })
    { }
  };


  /// @struct verify_cond_sub_delims
  /// @brief sub-delims - they are also "sub—delimiters"
  ///
  /// Thos symbols that divide the current large component into smaller components,
  /// they are different for each component.
  struct verify_cond_sub_delims final : public details_http_client_lite::verify_cond {
    verify_cond_sub_delims() noexcept
      : verify_cond({ '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=' })
    { }
  };


  /// @struct verify_cond_digit
  /// @brief digit - any number
  ///
  /// Sets of numbers to check (regExp [0-9])
  struct verify_cond_digit final : public details_http_client_lite::verify_cond {
    verify_cond_digit() noexcept
      : verify_cond({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' })
    { }
  };


  /// @struct verify_cond_alpha
  /// @brief alpha - any uppercase and lowercase letter of ASCII encoding
  ///
  /// Sets of ASCII encoding letters to check (regExp [A-Za-z])
  struct verify_cond_alpha final : public details_http_client_lite::verify_cond {
    verify_cond_alpha() noexcept
      : verify_cond({
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                      'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

                      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                      'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
                    })
    { }
  };


  /// @struct verify_cond_hexdig
  /// @brief hexdig - hexadecimal digit
  ///
  /// Sets of hexadecimal digit to check (regExp [0-9A-F])
  struct verify_cond_hexdig final : public details_http_client_lite::verify_cond {
    verify_cond_hexdig() noexcept
      : verify_cond({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'a', 'b', 'c', 'd', 'e', 'f',
                      'A', 'B', 'C', 'D', 'E', 'F'
                    })
    { }
  };

  /// @todo TODO: pct-encoded = "%" HEXDIG HEXDIG

  /// @class schem
  /// @brief URI Schema Name
  ///
  /// Each URI begins with a schema name that refers to the specification for assigning
  /// identifiers in that schema. Also, the URI syntax is a unified and extensible naming
  /// system, moreover, the specification of each scheme can further restrict the syntax
  /// and semantics of identifiers using this scheme.
  /// The name of the scheme must begin with a letter and can then be continued with any
  /// number of allowed characters.
  class schem final : public details_http_client_lite::uri_base {
    // Additional class with symbols to check
    struct verify_cond_others final : public details_http_client_lite::verify_cond {
      verify_cond_others() noexcept
        : verify_cond({ '+', '-', '.' })
      { }
    };

    using check_pack_t = details_http_client_lite::checker<verify_cond_alpha,
                                                           verify_cond_digit,
                                                           verify_cond_others>;

    details_http_client_lite::buff_offset data_;

  public:
    explicit schem(details_http_client_lite::buffer_t& buffer) noexcept
      : uri_base(buffer)
      , data_(this->get_token<0,10, check_pack_t>("://"))
    { }

    std::string to_str() const {
      return data_.get(this->buff_ref()).str();
    }
  };











//    class url {


//      details_http_client_lite::buffer<> buffer_;

//      details_http_client_lite::offseter<> schem_;

//    public:
//      explicit url(std::string const& url)
//        : buffer_(std::move(url))
//        , schem_(buffer_.template get_token<0, 10>("://"))
//      { }

//      std::string schem() noexcept {
//        return schem_.get(buffer_).str();
//      }
//    };




  struct uri {
    std::string protocol;
    std::string host;
    std::string port;
    std::string address;
    std::string query_string;
    std::string hash;

    string_map_t parameters;

    explicit uri(std::string input, bool shouldParseParameters = false) noexcept {
      tokenizer t = tokenizer(input);

      protocol = t.next("://");
      if (protocol.empty()) protocol = std::string("http", 4);

      std::string host_and_port = t.next("/");
      //      if (host_and_port.empty())
      //        host_and_port = t.tail();

      tokenizer hostPort(host_and_port);

      host = hostPort.next(host_and_port[0] == '[' ? "]:" : ":", true);

      if (host[0] == '[') host = host.substr(1, host.size() - 1);

      port = hostPort.tail();
      if (port.empty()) port = "80";

      address      = t.next("?", true);
      query_string = t.next("#", true);

      hash = t.tail();

      if (shouldParseParameters) {
        parseParameters();
      }
    }

    void parseParameters() {
      tokenizer qt(query_string);
      do {
        std::string key = qt.next("=");
        if (key == "") break;
        parameters[key] = qt.next("&", true);
      } while (true);
    }
  };

//    class scheme {
//      std::string name_ { "" };

//     public:
//      scheme() = default;
//      explicit scheme(const std::string& scheme_name) noexcept : name_ { scheme_name } {}

//      std::string name() const { return name_; }

//      template <size_t _start_pos>
//      void parse(const std::string& uri) {
//        using namespace std::placeholders;
//        using details_http_client_lite::get_token;

//        name_ = get_token<_start_pos>(uri, "://", std::bind(&scheme::check, this, _1));
//      }

//     protected:
//      // TODO: Checking on rull [A-Z,a-z] [0-9] [+ - .]
//      //       Don't start on number!!!
//      bool check(const std::string&) { return true; }
//    };

//    struct authority {
//      struct userinfo {
//        std::string name_ { "" };
//        std::string pass_ { "" };

//       public:
//        userinfo() = default;
//        explicit userinfo(const std::string& name, const std::string& pass) noexcept : name_ { name }, pass_ { pass } {}

//        std::string name() const { return name_; }
//        std::string pass() const { return pass_; }

//        template <size_t _start_pos>
//        void parse(const std::string& uri) {
//          using namespace std::placeholders;
//          using details_http_client_lite::get_token;

//          name_ = get_token<_start_pos>(uri, ":", std::bind(&userinfo::check, this, _1));

//          if (name_.empty()) name_ = get_token<_start_pos>(uri, "@", std::bind(&userinfo::check, this, _1));
//          else
//            pass_ = get_token<0>(name_, "@", std::bind(&userinfo::check, this, _1));
//        }

//       protected:
//        // TODO: Checking on rull: not reserved, precent-encoding, sub-delims, ":"
//        bool check(const std::string&) { return true; }
//      };
//      struct host {};
//    };

  struct path {};
  struct query {};
  struct fragment {};
}  // namespace uri





  namespace http {
    enum class resp_stat : uint8_t { success, failure };

    struct response {
      std::string protocol { "" };
      std::string resp { "" };
      std::string resp_str { "" };
      std::string body { "" };

      string_map_t header;

      resp_stat status { resp_stat::failure };

      static inline response fail() { return response(); }
    };

    enum class method : uint8_t { OPTIONS = 0, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT };

    static std::vector<std::string> methods = { "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT" };

    template <method __HTTP_TYPE>
    constexpr std::string& method2string() {
      return methods[static_cast<uint8_t>(__HTTP_TYPE)];
    }

    struct client {
      template <http::method __METHOD>
      static inline std::string gen_request(const uri::uri& uri, const std::string& body = "") {
        return method2string<__METHOD>() + " /" + uri.address + ((uri.query_string == "") ? "" : "?") +
               uri.query_string + " HTTP/1.1\r\n" + "Host: " + uri.host + ":" + uri.port + "\r\n" + "Accept: */*\r\n" +
               content_type + "\r\n" + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
      }
    };

  }  // namespace http

  struct HTTPClient {
    //    typedef enum {
    //      OPTIONS = 0,
    //      GET,
    //      HEAD,
    //      POST,
    //      PUT,
    //      DELETE,
    //      TRACE,
    //      CONNECT
    //    } HTTPMethod;

    //    inline static const char *method2string(HTTPMethod method) {
    //      const char *methods[] = {"OPTIONS", "GET",   "HEAD",    "POST", "PUT",
    //                               "DELETE",  "TRACE", "CONNECT", nullptr};
    //      return methods[method];
    //    }

    inline static int connectToURI(const uri::uri& uri) {
      struct addrinfo hints, *result, *rp;

      memset(&hints, 0, sizeof(addrinfo));

      hints.ai_family   = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;

      int getaddrinfo_result = getaddrinfo(uri.host.c_str(), uri.port.c_str(), &hints, &result);

      if (getaddrinfo_result != 0) return -1;

      socktype_t fd = INVALID_SOCKET;

      for (rp = result; rp != nullptr; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (fd == INVALID_SOCKET) {
          continue;
        }

        int connect_result = connect(fd, rp->ai_addr, rp->ai_addrlen);

        if (connect_result == -1) {
          // successfully created a socket, but connection failed. close it!
          closesocket(fd);
          fd = INVALID_SOCKET;
          continue;
        }

        break;
      }

      freeaddrinfo(result);

      return fd;
    }

    inline static std::string bufferedRead(int fd) {
      std::size_t initial_factor = 4, buffer_increment_size = 8192, buffer_size = 0, bytes_read = 0;
      std::string buffer;

      buffer.resize(initial_factor * buffer_increment_size);

      //    do {
      bytes_read = recv(fd, ((char*)buffer.c_str()) + buffer_size, buffer.size() - buffer_size, 0);

      buffer_size += bytes_read;

      //      if (bytes_read > 0 &&
      //          (buffer.size() - buffer_size) < buffer_increment_size) {
      //        buffer.resize(buffer.size() + buffer_increment_size);
      //      }
      //    } while (bytes_read > 0);

      buffer.resize(buffer_size);
      return buffer;
    }

#define HTTP_NEWLINE          "\r\n"
#define HTTP_SPACE            " "
#define HTTP_HEADER_SEPARATOR ": "

    template <http::method __METHOD>
    inline static http::response request(const uri::uri& uri, const std::string& body = "") {
      int fd = connectToURI(uri);
      if (fd < 0) return http::response::fail();

      //    string request = string(method2string(method)) + string(" /") +
      //                     uri.address + ((uri.querystring == "") ? "" : "?") +
      //                     uri.querystring + " HTTP/1.1" HTTP_NEWLINE "Host: " +
      //                     uri.host + HTTP_NEWLINE
      //                     "Accept: */*" HTTP_NEWLINE
      //                     "Connection: close" HTTP_NEWLINE HTTP_NEWLINE;

      std::string request = http::method2string<__METHOD>() + std::string(" /") + uri.address +
                            ((uri.query_string == "") ? "" : "?") + uri.query_string + " HTTP/1.1" + HTTP_NEWLINE +
                            "Host: " + uri.host + ":" + uri.port + HTTP_NEWLINE + "Accept: */*" + HTTP_NEWLINE +
                            content_type + HTTP_NEWLINE + "Content-Length: " + std::to_string(body.size()) +
                            HTTP_NEWLINE + HTTP_NEWLINE + body;

      /*int bytes_written = */ send(fd, request.c_str(), request.size(), 0);

      std::string buffer = bufferedRead(fd);

      close(fd);

      http::response result;

      tokenizer bt(buffer);

      result.protocol = bt.next(HTTP_SPACE);
      result.resp     = bt.next(HTTP_SPACE);
      result.resp_str = bt.next(HTTP_NEWLINE);

      std::string header = bt.next(HTTP_NEWLINE HTTP_NEWLINE);

      result.body = bt.tail();

      tokenizer ht(header);

      do {
        std::string key = ht.next(HTTP_HEADER_SEPARATOR);
        if (key == "") break;
        result.header[key] = ht.next(HTTP_NEWLINE, true);
      } while (true);

      return result;
    }
  };

JDL_NAMESPACE_END
