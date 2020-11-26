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
constexpr size_t hsmPrime_2 = 115249;
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

	constexpr void insert(const Key&& key, const Value&& val)
	{
		BuckPtr item = make_bucket(key, val);
		Size index = get_hash(item->s_key, m_table->size, 0);
		const bucket* cur = m_table->items.at(index).get();

		const Size tablesize = m_table->size;
		for (Size i = 0; !cur->s_key.empty() && i < tablesize; ++i) {
			index = get_hash(item->s_key, m_table->size, i);
			cur = m_table->items.at(index).get();
		}

		m_table->items[index] = std::move(item);
		++m_table->count;
	}

	constexpr Value search(const Key&& key) const
	{
		Size index = get_hash(key, m_table->size, 0);
		const bucket* item = m_table->items[index].get();

		const Size tablesize = m_table->size;
		for (Size i = 0; !item->s_key.empty() && i < tablesize; ++i) {
			if (item->s_key == key) {
				Value v = item->s_value;
				//m_table->items[index] = std::move(item);
				return v;
			}

			//m_table->items[index] = std::move(item);
			index = get_hash(key, m_table->size, i);
			item = m_table->items[index].get();
		}

		return "";
	}
	void del();

	constexpr BuckPtr make_bucket(const Key& k, const Value& v) const
	{
		BuckPtr i = this->make_ptr<bucket>();
		i->s_key = k;
		i->s_value = v;
		return i;
	}

	constexpr TablePtr make_table(Size MapSize = map_initial_size) const
	{
		TablePtr Table = make_ptr<hash_table>();
		Table->items.resize(MapSize);
		for (Size i = 0; i < MapSize; ++i) {
			Table->items[i] = (make_bucket(Key {}, Value {}));
		}
		Table->size = MapSize;
		Table->count = 0;
		return Table;
	};

	constexpr Size m_hash(Lengthy auto&& var, const Size prime, const Size modulo) const
	{
		Size _hash = 0;
		const Size len = var.length();
		for (Size i = 0; i < len; ++i) {
			_hash += static_cast<const Size>(std::pow(prime, len - (i + 1))) * static_cast<const Size>(var[i]);
		}
		_hash = static_cast<Size>(_hash % modulo + (modulo % modulo - 1)) * 2;
		return _hash + prime;
	}

	constexpr Size get_hash(Lengthy auto&& var, const Size num_bucks, const Size attempts) const
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

	constexpr void removeBucket(Size x)
	{
		if (m_table->items.at(x))
			m_table->items.at(x).release();
	}

	constexpr void removeTable(TablePtr& t)
	{
		if (t)
			t.release();
	}

public:
	struct bucket {
		Key s_key;
		Value s_value;
	};

	struct hash_table {
		uint64_t size;
		uint64_t count;
		MapVec items;
	};

	TablePtr m_table;
};
}
#endif // HASHMAP_H

#ifndef NDEBUG_MAIN

int main()
{
	auto tab = Li::HashMap<std::string, std::string>(1500);

	for (int i = 0; i != 1500; ++i) {
		tab.insert(std::to_string(i), std::to_string(i));
	}

	size_t misses = 0;
	for (int i = 0; i != 1500; ++i) {
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
