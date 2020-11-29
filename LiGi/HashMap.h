#ifndef HASHMAP_H
#define HASHMAP_H
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

template<typename T>
concept AcceptablePointer = requires(T a)
{
	{
		!a
	}
	->std::same_as<bool>;
	a.release();
	a.operator->();
	a.operator*();
	a.~T();
};

template<typename T>
concept DefaultConstructible = requires(T a)
{
	std::is_default_constructible_v<T>;
};

template<typename T>
concept hasCount = requires(T a)
{
	a.use_count();
};

struct pointer_factory {
public:
	using type = void;
	type make_ptr();
	virtual ~pointer_factory() = default;
};

template<DefaultConstructible T>
struct unique_pointer_factory : public pointer_factory {
public:
	using type = std::unique_ptr<T>;
	constexpr inline static type make_ptr()
	{
		return std::make_unique<T>();
	}
};

template<DefaultConstructible T>
struct shared_ptr_factory : public pointer_factory {
public:
	using type = std::shared_ptr<T>;

	constexpr inline static type make_ptr()
	{
		return std::make_shared<T>();
	}
};

template<template<DefaultConstructible X> class factory, typename X>
concept CanFactoryPTRs = requires(factory<X> a)
{
	{ a.make_ptr() };
	{ !std::is_same_v<typename factory<X>::type, void> == false };
	std::is_same_v<typename factory<X>::type, typename factory<X>::type>;
};

template<template<class> class factory, class X>
concept IsPTRFactory = std::is_base_of_v<pointer_factory, factory<X>>&& CanFactoryPTRs<factory, X>;

template<Lengthy Key,
	DefaultConstructible Value,
	template<class T> typename PTR_Factory = Li::shared_ptr_factory,
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
		_hash = static_cast<Size>(_hash % modulo + (modulo % modulo - 1)) * 2;
		return _hash + prime;
	}

	constexpr Size get_hash(Lengthy auto const& var, const Size& num_bucks, const Size& attempts) const
	{
		Size first = m_hash(var, hsmPrime_1, num_bucks);
		Size second = m_hash(var, hsmPrime_2, num_bucks);
		if ((second % num_bucks) == 0) {
			second = 1;
		}
		/*if ((first % num_bucks) == 0) {
			first = 2;
		}*/
		return (first + (attempts * (second))) % num_bucks;
	}

	constexpr void removeBucket(BuckPtr& it)
	{
		if constexpr (CLEANUP && hasCount<BuckPtr>) {
			if (it) {
				it = SENTINELBUCK;
			}
		}
	}

	constexpr static void removeTable(TablePtr& t)
	{
		if constexpr (CLEANUP) {
			if (t) {
				t.reset();
			}
		}
	}

	TablePtr m_table;

public:
	using SearchResult = std::variant<Value, NotFundError>;
	constexpr explicit HashMap(Size = map_initial_size) requires(!IsPTRFactory<PTR_Factory, bucket>) { }
	constexpr explicit HashMap(Size MapSize = map_initial_size) requires IsPTRFactory<PTR_Factory, bucket>
	{
		m_table = make_table(MapSize);
	}

	constexpr void insert(const Key&& key, const Value&& val)
	{
		BuckPtr item = make_bucket(key, val);
		Size index = get_hash(item->s_key, m_table->size, 0);
		const bucket* cur = m_table->items.at(index).get();

		const Size tablesize = m_table->size;
		const bucket* sentinel = SENTINELBUCK.get();
		Size i = 1;
		for (; i < tablesize && &sentinel != &cur && cur; ++i) {
			if (cur->s_key == item->s_key) {
				m_table->items[index].swap(item);
				return;
			}
			index = get_hash(item->s_key, m_table->size, i);
			cur = m_table->items.at(index).get();
		}

		/*if (cur != nullptr)
			std::cout << "Bail out "
					  << " at: " << i << ", hash: " << index << ", key: '" << key << "'\n";*/

		m_table->items[index].swap(item);
		++m_table->count;
	}

	constexpr SearchResult search(const Key&& key) const
	{
		Size index = get_hash(key, m_table->size, 0);
		const bucket* cur = m_table->items[index].get();

		const Size tablesize = m_table->size;
		for (Size i = 0; cur && i < tablesize; ++i) {
			const bucket* sentinel = SENTINELBUCK.get();
			if (&cur != &sentinel && cur->s_key == key) {
				Value v = cur->s_value;
				//m_table->items[index] = std::move(item);
				return v;
			}

			//m_table->items[index] = std::move(item);
			index = get_hash(key, m_table->size, i);
			cur = m_table->items[index].get();
		}

		return NotFundError { key };
	}

	void del(const Key&& key)
	{
		Size index = get_hash(key, m_table->size, 0);
		const bucket* item = m_table->items[index].get();
		const Size tablesize = m_table->size;
		for (Size i = 1; item && i < tablesize; ++i) {
			if (item && item->s_key == key) {
				removeBucket(m_table->items[index]); // sets memory to sentinel
				--m_table->size;
				return;
			}
			index = get_hash(key, m_table->size, i);
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

private:
	const static BuckPtr SENTINELBUCK;
	static_assert(IsPTRFactory<PTR_Factory, bucket>,
		"Delivered PTR_Factory(3rd. Argument) is not a valid Pointer factory!");
};

template<Lengthy Key, DefaultConstructible Value, template<class FT> class factory, bool CLEANUP>
const typename Li::HashMap<Key, Value, factory, CLEANUP>::BuckPtr
	Li::HashMap<Key, Value, factory, CLEANUP>::SENTINELBUCK
	= Li::HashMap<Key, Value, factory, CLEANUP>::make_bucket(Key {}, Value {});
}
#endif // HASHMAP_H

#ifndef NDEBUG_MAIN

int main()
{
	auto tab = Li::HashMap<std::string, std::string, Li::shared_ptr_factory>(15000);

	for (int i = 0; i != 15000; ++i) {
		tab.insert(std::to_string(i), std::to_string(i));
	}

	size_t misses = 0;
	for (int i = 0; i != 15000; ++i) {
		try {
			auto x = std::get<std::string>(tab.search(std::to_string(i)));
		} catch (std::bad_variant_access& x) {
			++misses;
		}
	}
	tab.insert("brot", "war");
	tab.insert("brats", "world");
	tab.insert("brigitte", "welt");

	tab.del("brot");
	tab.del("brot");
	tab.del("brats");
	tab.insert("brot", " goes brrr hahahaha");
	std::cout << "brot: " << std::get<std::string>(tab.search("brot"));
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
	/*for (auto& b : tab.m_table->items) {
		if (!b && b->s_key.empty())
			Empty_Buckets++;
		else
			Full_Buckets++;
	}

	*/
	std::cout << "Full: " << Full_Buckets << ", Empty: " << Empty_Buckets << ", Misses: " << misses << std::endl;

	/*for (size_t x = 0; x < tab.m_table->items.size(); ++x) {
		auto b = tab.m_table->items[x]->s_key;
		auto a = tab.m_table->items[x]->s_value;
		std::cout << "Key: " << b
				  << "\nValue: " << a << "\n";
	}*/

	std::cout << "Sentinel Use: " << tab.SentinelUse();
}
#endif
