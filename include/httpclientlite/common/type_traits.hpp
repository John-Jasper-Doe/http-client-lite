
#pragma once

/// @file type_traits.hpp
/// @brief Backward-compatible type traits for C++11/14/17
///
/// Provides common type trait aliases that are available in C++14/17,
/// but with fallback definitions for C++11 compilers.

#include <type_traits>

/// @namespace common
namespace common {

// C++17 and later - _t aliases are in std::
#if __cplusplus >= 201703L

	/// @brief SFINAE helper - equivalent to std::enable_if_t
	template <bool COND, typename TYPE = void>
	using enable_if_t = typename std::enable_if_t<COND, TYPE>;

	/// @brief Removes const/volatile qualifiers - equivalent to std::remove_cv_t
	template <typename TYPE>
	using remove_cv_t = typename std::remove_cv_t<TYPE>;

	/// @brief Removes pointer indirection - equivalent to std::remove_pointer_t
	template <typename TYPE>
	using remove_pointer_t = typename std::remove_pointer_t<TYPE>;

// C++14 - _t aliases are in std::
#elif __cplusplus >= 201402L

	/// @brief SFINAE helper - equivalent to std::enable_if_t
	template <bool COND, typename TYPE = void>
	using enable_if_t = typename std::enable_if_t<COND, TYPE>;

	/// @brief Removes const/volatile qualifiers - equivalent to std::remove_cv_t
	template <typename TYPE>
	using remove_cv_t = typename std::remove_cv_t<TYPE>;

	/// @brief Removes pointer indirection - equivalent to std::remove_pointer_t
	template <typename TYPE>
	using remove_pointer_t = typename std::remove_pointer_t<TYPE>;

#else // C++11 fallback - _t aliases don't exist in std::

	/// @brief SFINAE helper - equivalent to std::enable_if_t (C++14)
	template <bool COND, typename TYPE = void>
	using enable_if_t = typename std::enable_if<COND, TYPE>::type;

	/// @brief Removes const/volatile qualifiers - equivalent to std::remove_cv_t (C++14)
	template <typename TYPE>
	using remove_cv_t = typename std::remove_cv<TYPE>::type;

	/// @brief Removes pointer indirection - equivalent to std::remove_pointer_t (C++14)
	template <typename TYPE>
	using remove_pointer_t = typename std::remove_pointer<TYPE>::type;

#endif /* Check C++ Standard */


	/// @brief Compares a type (after stripping cv-qualifiers and pointer) against a target type
	/// @tparam CHECK_TYPE - type to check (cv-qualifiers and pointer are stripped before comparison)
	/// @tparam TYPE - required type to compare against
	template <typename CHECK_TYPE, typename TYPE>
	using is_require_type = std::is_same<remove_cv_t<remove_pointer_t<CHECK_TYPE>>, TYPE>;

	/// @brief Default (false) specialization - type is not a character type
	template <typename, typename CHECK = void>
	struct is_char_type : std::false_type {};

	/// @brief True specialization - type is char, wchar_t, char16_t, or char32_t
	template <typename TYPE>
	struct is_char_type<
		TYPE,
		enable_if_t<is_require_type<TYPE, char>::value || is_require_type<TYPE, wchar_t>::value ||
								is_require_type<TYPE, char16_t>::value || is_require_type<TYPE, char32_t>::value>>
		: std::true_type {};

} // namespace common
