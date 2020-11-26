#ifndef CONCEPTS_ADDITIONS_H
#define CONCEPTS_ADDITIONS_H
#pragma once

#include <concepts>
#include <unordered_map>
namespace concepts {

template<typename T>
concept EqualKeyValue = std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>>&& std::same_as<typename T::value_type::second_type, typename T::key_type>;

template<typename T>
concept NotEqualKeyValue = std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>> && !std::same_as<typename T::value_type::second_type, typename T::key_type>;

template<typename T>
concept Numeric = (std::is_unsigned_v<T> || std::is_signed_v<T>)&&std::is_convertible_v<T, int>&& std::is_convertible_v<T, int>;

template<typename T>
concept GeneralIterator = requires(T a)
{
	{
		a.operator++(0)
	}
	->std::convertible_to<T&>;

	{
		a.operator++()
	}
	->std::convertible_to<T&>;

	{
		a.operator!=(a)
	}
	->std::same_as<bool>;

	{
		a.operator*()
	}
	->std::convertible_to<typename T::_type&>;

	{
		a.operator->()
	}
	->std::convertible_to<typename T::_type*>;
};

}

// -----------------------------------------------------------------------------
#endif // CONCEPTS_ADDITIONS_H
