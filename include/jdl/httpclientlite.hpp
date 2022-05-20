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

const std::string content_type { "Content-Type: text/plain; version=0.0.4; charset=utf-8" };

const std::string http_new_line { "\r\n" };
const std::string http_space { " " };
const std::string http_header_separator { ": " };

namespace jdl {

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
  constexpr bool is_same_v = std::is_same_v<TYPE, COMPARED_TYPE>;

  template <typename TYPE>
  using remove_cv_t = typename std::remove_cv_t<TYPE>;

#elif __cplusplus >= 201402L  // If C++ Standart == 14

  template <typename TYPE>
  using is_integral_v = std::is_integral_v<TYPE>;

  template <bool COND, typename TYPE = void>
  using enable_if_t = typename std::enable_if_t<COND, TYPE>;

  template<typename TYPE, typename COMPARED_TYPE >
  constexpr bool is_same_v = std::is_same<TYPE, COMPARED_TYPE>::value;

  template <typename TYPE>
  using remove_cv_t = typename std::remove_cv_t<TYPE>;

#else  // If C++ Standart == 11

  // Trates
  template <typename TYPE>
  constexpr bool is_integral_v = std::is_integral<TYPE>::value;

  template <bool COND, typename TYPE = void>
  using enable_if_t = typename std::enable_if<COND, TYPE>::type;

  template<typename TYPE, typename COMPARED_TYPE >
  constexpr bool is_same_v = std::is_same<TYPE, COMPARED_TYPE>::value;

  template <typename TYPE>
  using remove_cv_t = typename std::remove_cv<TYPE>::type;

#endif /* Check C++ Standart */

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

  /** @namespace external_dependencies */
  namespace external_dependencies {

    /** @class string_chunk
     *  @brief Implementation of the string_view micro class
     */
    template <typename CHAR_TYPE = char>
    class string_chunk {
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

      const_reference operator[](size_type pos) const { return get_on_pos(pos); }

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

    protected:
      size_type to_pos(const_reverse_iterator it) const {
        return (it == crend()) ? std::string::npos : size_type(crend() - it - 1);
      }

      size_type to_pos(const_iterator it) const {
        return (it == cend()) ? std::string::npos : size_type(it - cbegin());
      }

      const_reference get_on_pos(size_type pos) const {
        assert (pos < size());
        return data_[pos];
      }
    };



  } /* external_dependencies:: */

  /** @namespace details_http_client_lite */
  namespace details_http_client_lite {


    template <typename TYPE = std::string,
              typename = jdl::enable_if_t<jdl::is_same_v<jdl::remove_cv_t<TYPE>, TYPE>, bool>>
    using checker_t = std::function<bool(TYPE const&)>;

    using string_chunk_t = external_dependencies::string_chunk<>;

    template <typename OFFSET_TYPE>
    class offseter;

    /** @class buffer */
    class buffer {
      std::string const str_;

    public:
      buffer() = delete;
      explicit buffer(std::string const& str) noexcept : str_(std::move(str)) { }

      bool is_empty() noexcept { return str_.empty(); }

      size_t             size() const noexcept { return str_.size(); }
      std::string const& str()  const noexcept { return str_; }
      char const*        data() const noexcept { return str_.data(); }

      template <typename OFFSET_TYPE>
      offseter<OFFSET_TYPE> get_offseter(std::string const& str) {
        size_t pos = str_.find(str);
        if (pos != std::string::npos) {
          return offseter<OFFSET_TYPE>(static_cast<OFFSET_TYPE>(pos), str.size());
        }

        return offseter<OFFSET_TYPE>();
      }

      template <typename OFFSET_TYPE, size_t POSITION, size_t COUNT>
      offseter<OFFSET_TYPE> get_token(std::string const& delim, checker_t<string_chunk_t> checker) {
        string_chunk_t cnk(str_.data() + POSITION, COUNT);

        size_t pos = cnk.find(delim);
        if (pos != std::string::npos && checker != nullptr &&
            checker(string_chunk_t(str_.data() + POSITION + pos, delim.size()))) {
          return offseter<OFFSET_TYPE>(static_cast<OFFSET_TYPE>(pos), delim.size());
        }

        return offseter<OFFSET_TYPE>();
      }
    };

    /** @class offeter */
    template <typename OFFSET_TYPE = uint16_t>
    class offseter final {
      static_assert(jdl::is_integral_v<OFFSET_TYPE>, "Wrong type, should be integral");

    public:
      using offset_type        = OFFSET_TYPE;
      using size_type          = size_t;
      using internel_size_type = OFFSET_TYPE;

    private:
      OFFSET_TYPE offset_ = 0;
      OFFSET_TYPE size_   = 0;

    public:
      offseter() = default;
      offseter(offset_type offset, offset_type size) noexcept : offset_(offset), size_(size) { }

      bool        is_empty() const noexcept { return (offset_ == 0 && size_ == 0); }

      size_type   size()     const noexcept { return static_cast<size_type>(size_); }
      offset_type offset()   const noexcept { return offset_; }

      string_chunk_t get(buffer const& buff) const noexcept {
        return (buff.size() > (offset_ + size_))
            ? string_chunk_t(buff.data() + offset_, size_)
            : string_chunk_t();
      }

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

    std::ostream& operator<<(std::ostream& os, offseter<> const& of) {
        return (os << "[ offset: " << of.offset_ << ", size: " << of.size_);
    }



    template <size_t START_POSITION>
    inline std::string get_token(const std::string& str, const std::string& delim, checker_t<> checker) {
      std::size_t hit = str.find(delim, START_POSITION);
      if (hit != std::string::npos) {
        if (checker(str.substr(START_POSITION, hit - START_POSITION)))
          return str.substr(START_POSITION, hit - START_POSITION);
      }

      return std::string();
    }

  }  /* details_http_client_lite:: */






  namespace uri {

    using url_t   = details_http_client_lite::buffer;
    using token_t = details_http_client_lite::offseter<>;



    class url {
      std::string url_;

      url(const std::string& url) : url_(std::move(url)) {}
    };

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

    class scheme {
      std::string name_ { "" };

     public:
      scheme() = default;
      explicit scheme(const std::string& scheme_name) noexcept : name_ { scheme_name } {}

      std::string name() const { return name_; }

      template <size_t _start_pos>
      void parse(const std::string& uri) {
        using namespace std::placeholders;
        using details_http_client_lite::get_token;

        name_ = get_token<_start_pos>(uri, "://", std::bind(&scheme::check, this, _1));
      }

     protected:
      // TODO: Checking on rull [A-Z,a-z] [0-9] [+ - .]
      //       Don't start on number!!!
      bool check(const std::string&) { return true; }
    };

    struct authority {
      struct userinfo {
        std::string name_ { "" };
        std::string pass_ { "" };

       public:
        userinfo() = default;
        explicit userinfo(const std::string& name, const std::string& pass) noexcept : name_ { name }, pass_ { pass } {}

        std::string name() const { return name_; }
        std::string pass() const { return pass_; }

        template <size_t _start_pos>
        void parse(const std::string& uri) {
          using namespace std::placeholders;
          using details_http_client_lite::get_token;

          name_ = get_token<_start_pos>(uri, ":", std::bind(&userinfo::check, this, _1));

          if (name_.empty()) name_ = get_token<_start_pos>(uri, "@", std::bind(&userinfo::check, this, _1));
          else
            pass_ = get_token<0>(name_, "@", std::bind(&userinfo::check, this, _1));
        }

       protected:
        // TODO: Checking on rull: not reserved, precent-encoding, sub-delims, ":"
        bool check(const std::string&) { return true; }
      };
      struct host {};
    };

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

}  // namespace jdl
