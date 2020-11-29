#ifndef HASHMAP_H
#define HASHMAP_H
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

namespace Li {

constexpr size_t map_initial_size = 53;
constexpr size_t hsmPrime_1 = 8765753;
constexpr size_t hsmPrime_2 = 6660499;
using Size = size_t;

/// @brief Concept which requires default constructiblity, .lengt(), operator[], operator<<(ostream&), and operator==(ownType)
template<typename T>
concept Lengthy = requires(T a)
{
	std::is_default_constructible_v<T>;

	{
		a.length()
	}
	->std::convertible_to<Size>;

	{
		a == a
	}
	->std::same_as<bool>;

	{
		a.operator[](0)
	};

	std::cout << a;
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

/// @brief Concept requiring default constructibility
template<typename T>
concept DefaultConstructible = requires(T a)
{
	std::is_default_constructible_v<T>;
};

/// @brief Concept checking for .use_count() method
template<typename T>
concept hasCount = requires(T a)
{
	a.use_count();
};

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
template<DefaultConstructible T>
struct unique_pointer_factory : public pointer_factory<std::unique_ptr<T>> {
public:
	constexpr inline static std::unique_ptr<T> make_ptr()
	{
		return std::make_unique<T>();
	}

	template<typename... Args>
	constexpr inline static std::unique_ptr<T> make_ptr(Args... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
};

/**
 * @brief creates shared_ptr `s
 */
template<DefaultConstructible T>
struct shared_ptr_factory : public pointer_factory<std::shared_ptr<T>> {
public:
	constexpr inline static std::shared_ptr<T> make_ptr()
	{
		return std::make_shared<T>();
	}

	template<typename... Args>
	constexpr inline static std::shared_ptr<T> make_ptr(Args... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
};

/// @brief can create pointers with a make_ptr function, also storing it's type
template<template<DefaultConstructible X> class factory, typename X>
concept ProducesPointers = requires(factory<X> a)
{
	{ a.make_ptr() };
	{ !std::is_same_v<typename factory<X>::type, void> == false };
	std::is_same_v<typename factory<X>::type, typename factory<X>::type>;
};

/// @brief Is child of pointer_factory, can create an acceptable pointer for the hash map
template<template<class> class factory, class X>
concept PointerFactory = (std::is_base_of_v<pointer_factory<typename factory<X>::type>, factory<X>>)&&(ProducesPointers<factory, X>)&&(AcceptablePointer<decltype(factory<X>::make_ptr())>);

template<Lengthy Key,
	DefaultConstructible Value,
	template<DefaultConstructible T> typename PTR_Factory = Li::shared_ptr_factory,
	bool CLEANUP = true>
class HashMap {

protected:
	struct NotFundError {
		std::string field;
	};
	struct bucket;
	struct hash_table;
	template<typename T>
	using ptr = typename PTR_Factory<T>::type;

	using KeyPtr = ptr<Key>;
	using ValuePtr = ptr<Value>;

	using BuckPtr = ptr<bucket>;
	using MapVec = std::vector<BuckPtr>;
	using TablePtr = ptr<hash_table>;

	using ptrsize = ptrdiff_t;

	struct bucket {
		Key s_key;
		Value s_value;
	};

	struct hash_table {
		uint64_t size;
		uint64_t count;
		MapVec items;
	};

	static constexpr BuckPtr make_bucket(const Key& k, const Value& v)
	{
		BuckPtr i = PTR_Factory<bucket>::make_ptr();
		i->s_key = k;
		i->s_value = v;
		return i;
	}

	constexpr TablePtr make_table(Size MapSize = map_initial_size) const
	{
		TablePtr Table = PTR_Factory<hash_table>::make_ptr();
		Table->items.resize(MapSize);
		/* Initialise Array with empty values in buckets
		   for (Size i = 0; i < MapSize; ++i) {
			Table->items[i] = (make_bucket(Key {}, Value {}));
		}*/
		Table->size = MapSize;
		Table->count = 0;
		return Table;
	};

	constexpr Size m_hash(Lengthy auto const& var, const Size& prime, const Size& modulo) const
	{
		Size _hash = 0;
		const Size len = var.length();
		for (Size i = 0; i < len; ++i) {
			_hash += static_cast<Size>(std::pow(prime, len - (i + 1))) * static_cast<const Size>(var[i]);
		}
		_hash = static_cast<Size>(_hash % modulo);
		return _hash;
	}

	constexpr Size get_hash(Lengthy auto const& var, const Size& num_bucks, const Size& attempts) const
	{
		Size first = m_hash(var, hsmPrime_1, num_bucks);
		Size second = m_hash(var, hsmPrime_2, num_bucks);
		if ((second % num_bucks) == 0) {
			second = 1;
		}
		return (first + (attempts * (second))) % num_bucks;
	}

	void removeBucket(BuckPtr& it)
	{
		if constexpr (!CLEANUP || !hasCount<BuckPtr>) { // when we can't ref-count, we cant replace it with a ref
			return;
		}
		if (it) {
			it = SENTINELBUCK;
		}
	}

	constexpr static void removeTable(TablePtr& t)
	{
		if constexpr (!CLEANUP) {
			return;
		}
		if (t) {
			t.reset();
		}
	}

	TablePtr m_table;

public:
	using SearchResult = std::variant<Value, NotFundError>;
	constexpr explicit HashMap() requires(PointerFactory<PTR_Factory, bucket>) = delete;
	constexpr HashMap(HashMap&& other)
	{
		if (&other == this)
			return;
		m_table = std::move(other.m_table);
		other.m_table.reset();
	}

	HashMap(HashMap& other)
	{
		if (&other == this)
			return;

		removeTable(m_table);
		m_table = make_table(other.m_table->items.size());

		// deep-copy whole frigging vector
		std::copy(other.m_table->items.begin(), other.m_table->items.end(), std::back_inserter(m_table->items));
		m_table->size = other.m_table->size;
		m_table->count = other.m_table->count;
	}

	HashMap& operator=(HashMap const& other)
	{
		if (this == &other)
			return *this;
		*this(other);
	}

	constexpr explicit HashMap(Size MapSize = map_initial_size) requires PointerFactory<PTR_Factory, bucket>
	{
		m_table = make_table(MapSize);
	}

	constexpr void insert(const Key&& key, const Value&& val)
	{
		BuckPtr item = make_bucket(key, val);
		Size index = get_hash(item->s_key, m_table->size, 0);
		bucket* cur = m_table->items.at(index).get();

		const bucket* sentinel = SENTINELBUCK.get();
		for (Size i = 1; cur != nullptr && (&sentinel != &cur); ++i) {
			if (cur->s_key == key) {
				m_table->items[index].swap(item);
				return;
			}
			index = get_hash(item->s_key, m_table->size, i);
			cur = m_table->items.at(index).get();
		}

		/*if (cur != nullptr)
			std::cout << "Bail out "
					  << " at: " << i << ", hash: " << index << ", key: '" << key << "'\n";*/

		m_table->items[index]
			= std::move(item);
		++m_table->count;
	}

	constexpr SearchResult
	search(const Key& key) const
	{
		Size index = get_hash(key, m_table->size, 0);
		const bucket* cur = m_table->items[index].get();

		for (Size i = 1; cur; ++i) {
			const bucket* sentinel = SENTINELBUCK.get();
			if (cur && &cur != &sentinel) {
				if (cur->s_key == key) {
					Value v = cur->s_value;
					//m_table->items[index] = std::move(item);
					return v;
				}
			}

			//m_table->items[index] = std::move(item);
			index = get_hash(key, m_table->size, i);
			cur = m_table->items[index].get();
		}

		return NotFundError { key };
	}

	void del(const Key& key)
	{
		const Size tablesize = m_table->size;
		Size index = get_hash(key, tablesize, 0);
		bucket* item = m_table->items[index].get();
		bucket* sentinel = SENTINELBUCK.get();

		for (Size i = 1; i < tablesize; ++i) {
			if (item != sentinel && item != nullptr) {
				if (item->s_key == key) {
					--m_table->size;
					removeBucket(m_table->items[index]); // sets memory to sentinel
					return;
				}
			}
			index = get_hash(key, tablesize, i);
			item = m_table->items[index].get();
		}
	}

	constexpr ptrsize SentinelUse() requires(hasCount<BuckPtr>) { return SENTINELBUCK.use_count(); }
	constexpr ptrsize SentinelUse() noexcept(false) requires(!hasCount<BuckPtr>)
	{
		using namespace std::literals;
		throw std::logic_error("Currently used (smart)pointer '"s + typeid(decltype(BuckPtr {})).name() + "' has no ref-counting capabilites!"s);
		return 0;
	}

	virtual ~HashMap() { removeTable(m_table); }

	const static BuckPtr SENTINELBUCK;
	static_assert(PointerFactory<PTR_Factory, bucket>,
		"Delivered PTR_Factory(3rd. Argument) is not a valid Pointer factory!");
#ifndef NDEBUG
	void dbg(Size testsize)
	{

		m_table.reset();
		Size s = (testsize / 3);
		m_table = make_table(testsize);

		for (size_t i = 0; i != s; ++i) {
			insert(std::to_string(i), std::to_string(i));
		}

		size_t misses = 0;
		for (size_t i = 0; i != s; ++i) {
			try {
				auto y = std::to_string(i);
				auto x = std::get<std::string>(search(y));
				assert((x != y) || static_cast<bool>("Debug fail: hash-search not perfect"));
			} catch (std::bad_variant_access& x) {
				++misses;
			}
		}
		insert("brot", "war");
		insert("brats", "world");
		insert("brigitte", "welt");
		insert("hash", "map");

		del("brot");
		del("hash");
		del("brigitte");
		del("brats");
		insert("brot", " goes brrr hahahaha");
		insert("brot", "second");
		std::cout << "brot: " << std::get<std::string>(search("brot"));
		std::cout << std::endl;

		//tab.removeBucket(3);

		/*for (auto&& b : tab.m_table->items)
	{
		if (b)
			std::cout << "Valid -> ";
		else
			std::cout << "Invalid -> ";
	}
	std::cout << "." << std::endl;
	*/

		size_t Empty_Buckets = 0;
		size_t Full_Buckets = 0;
		for (auto& b : m_table->items) {
			if (!b || b == SENTINELBUCK)
				Empty_Buckets++;
			else
				Full_Buckets++;
		}

		std::cout << "Given Buckets((testsize/3)+testsize): " << testsize
				  << "\nFull Buckets: " << Full_Buckets
				  << ", Empty Buckets: " << Empty_Buckets
				  << " = " << (Full_Buckets + Empty_Buckets) << "! "
				  << "Search Misses: " << misses << std::endl;
		std::cout << "Sentinel Use: " << SentinelUse() << "\n";
	}
#endif
};

template<Lengthy Key, DefaultConstructible Value, template<DefaultConstructible FT> class factory, bool CLEANUP>
const typename Li::HashMap<Key, Value, factory, CLEANUP>::BuckPtr
	Li::HashMap<Key, Value, factory, CLEANUP>::SENTINELBUCK
	= Li::HashMap<Key, Value, factory, CLEANUP>::make_bucket(Key {}, Value {});
}
#endif // HASHMAP_H

#ifndef NDEBUG_MAIN

int main()
{
	auto tab = Li::HashMap<std::string, std::string, Li::shared_ptr_factory>(20000);

	tab.dbg(15000);

	/*for (size_t x = 0; x < tab.m_table->items.size(); ++x) {
		auto b = tab.m_table->items[x]->s_key;
		auto a = tab.m_table->items[x]->s_value;
		std::cout << "Key: " << b
				  << "\nValue: " << a << "\n";
	}*/
}
#endif
