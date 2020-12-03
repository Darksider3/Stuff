#ifndef PTR_FACTORY_H
#define PTR_FACTORY_H
#include "concepts_additions.h"
#include <concepts>
#include <memory>

namespace Li {
/**
 * @brief Base class for factories... factoring... pointers.
 */
template<class T>
struct pointer_factory {
public:
	using type = T;
	type make_ptr();
	virtual ~pointer_factory() = default;
};

/**
 * @brief Creates unique_ptr `s
 */
template<concepts::DefaultConstructible T>
struct unique_pointer_factory : public pointer_factory<std::unique_ptr<T>> {
public:
	constexpr inline static std::unique_ptr<T> make_ptr()
	{
		return std::move(std::make_unique<T>());
	}

	template<typename... Args>
	constexpr inline static std::unique_ptr<T> make_ptr(Args... args)
	{
		return std::move(std::make_unique<T>(std::forward<Args>(args)...));
	}
};

/**
 * @brief creates shared_ptr `s
 */
template<concepts::DefaultConstructible T>
struct shared_ptr_factory : public pointer_factory<std::shared_ptr<T>> {
public:
	constexpr inline static std::shared_ptr<T> make_ptr()
	{
		return std::move(std::make_shared<T>());
	}

	template<typename... Args>
	constexpr inline static std::shared_ptr<T> make_ptr(Args... args)
	{
		return std::move(std::make_shared<T>(std::forward<Args>(args)...));
	}
};

/// @brief Concept for acceptable pointers
template<typename T>
concept AcceptablePointer = requires(T a)
{
	{
		!a
	}
	->std::same_as<bool>; // has operator bool
	a.operator->();       // can dereference
	a.operator*();        // can dereference
	a.operator=(a);       // can assign pointer
	a.swap(a);            // can swap pointers
	a.reset();            // can reset pointer
	a.get();              // can get pointer
	a.~T();               // can deallocate
};

/// @brief Is child of pointer_factory, can create an acceptable pointer for the hash map
template<template<class> class factory, class X>
concept PointerFactory = (std::is_base_of_v<pointer_factory<typename factory<X>::type>, factory<X>>)&&(concepts::ProducesPointers<factory, X>)&&(AcceptablePointer<decltype(factory<X>::make_ptr())>);
}
#endif // PTR_FACTORY_H
