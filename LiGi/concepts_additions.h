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

/// @brief Concept checking for .use_count() method
template<typename T>
concept UseCounter = requires(T a)
{
	a.use_count();
};

/// @brief Concept requiring default constructibility
template<typename T>
concept DefaultConstructible = requires(T a)
{
	std::is_default_constructible_v<T>;
};

/// @brief can create pointers with a make_ptr function, also storing it's type
template<template<DefaultConstructible X> class factory, typename X>
concept ProducesPointers = requires(factory<X> a)
{
	{ a.make_ptr() };
	{ factory<X>::make_ptr() };
	{ !std::is_same_v<typename factory<X>::type, void> == false };
	std::is_same_v<typename factory<X>::type, typename factory<X>::type>;
};

}

// -----------------------------------------------------------------------------
#endif // CONCEPTS_ADDITIONS_H
