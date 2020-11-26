#ifndef HASHMAP_H
#define HASHMAP_H
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include <iostream>

namespace Li {
#include <concepts>

constexpr size_t map_initial_size = 53;
constexpr size_t hsmPrime_1 = 9239;
constexpr size_t hsmPrime_2 = 35317;
using Size = size_t;
#define FUN_ALIAS(NEW_NAME, ...)                                                                                            \
	inline static auto NEW_NAME = [](auto&&... args) noexcept(noexcept(__VA_ARGS__(std::forward<decltype(args)>(args)...))) \
		-> decltype(auto) {                                                                                                 \
		return __VA_ARGS__(std::forward<decltype(args)>(args)...);                                                          \
	}
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
};
template<Lengthy Key, typename Value>
class HashMap {
public:
	struct bucket;
	struct hash_table;

	template<typename T>
	using ptr = std::unique_ptr<T>;

	template<typename T>
	constexpr inline static ptr<T> make_ptr()
	{
		return std::make_unique<T>();
	}

	using BuckPtr = ptr<bucket>;
	using KeyPtr = ptr<Key>;
	using ValuePtr = ptr<Value>;
	using MapVec = std::vector<BuckPtr>;
	using TablePtr = ptr<hash_table>;

public:
	constexpr explicit HashMap(Size MapSize = map_initial_size)
	{
		m_table = make_table(MapSize);
	}

	constexpr void insert(Key key, Value val)
	{
		BuckPtr item = make_bucket(key, val);
		Size index = get_hash(item->s_key, m_table->size, 0);
		bucket* cur = m_table->items.at(index).get();
		for (Size i = 0; !cur->s_key.empty() && i != m_table->size; i++) {
			index = get_hash(item->s_key, m_table->size, i);
			cur = m_table->items.at(index).get();
		}

		m_table->items[index] = std::move(item);
		m_table->count++;
	}

	constexpr Value search(Key key)
	{
		Size index = get_hash(key, m_table->size, 0);
		BuckPtr old;
		bucket* item = m_table->items[index].get();
		for (Size i = 0; !item->s_key.empty() && i != m_table->size; ++i) {
			if (item->s_key == key) {
				Value v = item->s_value;
				//m_table->items[index] = std::move(item);
				return v;
			}

			//m_table->items[index] = std::move(item);
			index = get_hash(key, m_table->size, i);
			item = m_table->items[index].get();
		}

		return nullptr;
	}
	void del();

public:
	struct bucket {
		Key s_key;
		Value s_value;
	};

	static BuckPtr make_bucket(const Key k, const Value v)
	{
		BuckPtr i = std::make_unique<bucket>();
		i->s_key = k;
		i->s_value = v;
		return i;
	}
	struct hash_table {
		uint64_t size;
		uint64_t count;
		MapVec items;
	};

	static TablePtr make_table(Size MapSize = map_initial_size)
	{
		TablePtr Table = std::make_unique<hash_table>();
		Table->items.resize(MapSize);
		for (Size i = 0; i < MapSize; ++i) {
			Table->items[i] = (make_bucket(Key {}, Value {}));
		}
		Table->size = 53;
		Table->count = 0;
		return Table;
	};

	static Size m_hash(Lengthy auto&& var, Size prime, Size modulo)
	{
		Size _hash = 0;
		const Size len = var.length();
		for (Size i = 0; i < len; ++i) {
			_hash += static_cast<Size>(std::pow(prime, len - (i + 1))) * static_cast<Size>(var[i]);
			_hash = static_cast<Size>(_hash % modulo);
		}
		return _hash;
	}

	static Size get_hash(Lengthy auto&& var, const Size num_bucks, const Size attempts)
	{
		const Size first = m_hash(var, hsmPrime_1, num_bucks);
		const Size second = m_hash(var, hsmPrime_2, num_bucks);
		return (first + (attempts * (second + 1))) % num_bucks;
	}

	void removeBucket(Size x)
	{
		if (m_table->items.at(x))
			m_table->items.at(x).release();
	}

	void removeTable(TablePtr& t)
	{
		if (t)
			t.release();
	}

	TablePtr m_table;
};
}
#endif // HASHMAP_H

#ifndef NDEBUG_MAIN

int main()
{
	auto tab = Li::HashMap<std::string, std::string>(15000);

	for (int i = 0; i != 15000; ++i) {
		tab.insert(std::to_string(i), std::to_string(i));
	}

	size_t misses = 0;
	for (int i = 0; i != 15000; ++i) {
		if (tab.search(std::to_string(i)).empty())
			++misses;
	}
	tab.insert("brot", "world");
	tab.insert("brigitte", "welt");

	std::cout << tab.m_table->items.size() << "brot: " << tab.search("brot");
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
	for (auto& b : tab.m_table->items) {
		if (b->s_key.empty())
			Empty_Buckets++;
		else
			Full_Buckets++;
	}

	std::cout << "Full: " << Full_Buckets << ", Empty: " << Empty_Buckets << ", Misses: " << misses << std::endl;

	/*for (size_t x = 0; x < tab.m_table->items.size(); ++x) {
		auto b = tab.m_table->items[x]->s_key;
		auto a = tab.m_table->items[x]->s_value;
		std::cout << "Key: " << b
				  << "\nValue: " << a << "\n";
	}*/

	tab.removeTable(tab.m_table);
	if (tab.m_table)
		std::cout << "still there that table \n";
	else
		std::cout << "table GONE. \n";
}
#endif
