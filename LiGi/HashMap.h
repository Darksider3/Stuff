#ifndef HASHMAP_H
#define HASHMAP_H
#include <cstdint>
#include <memory>
#include <vector>

#include <iostream>

namespace Li {
constexpr size_t map_initial_size = 53;
using Size = size_t;
template<typename Key, typename Value>
class HashMap {
public:
public:
	struct bucket;
	struct hash_table;

	template<typename T>
	using ptr = std::unique_ptr<T>;

	using BuckPtr = ptr<bucket>;
	using KeyPtr = ptr<Key>;
	using ValuePtr = ptr<Value>;
	using MapVec = std::vector<BuckPtr>;
	using TablePtr = ptr<hash_table>;

	struct bucket {
		Value s_key;
		Key s_value;
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

	static TablePtr make_table()
	{
		TablePtr Table = std::make_unique<hash_table>();
		Table->items.resize(map_initial_size);
		for (Size i = 0; i < map_initial_size; ++i) {
			Table->items[i] = (make_bucket(Key {}, Value {}));
		}
		Table->size = 0;
		Table->count = 0;
		return Table;
	};

	void removeBucket(Size x)
	{
		if (m_table->items.at(x))
			m_table->items.at(x).release();
	}

	TablePtr m_table;
};
}
#endif // HASHMAP_H

#ifndef NDEBUG_MAIN

int main()
{
	auto tab = Li::HashMap<char, char>();
	tab.m_table = Li::HashMap<char, char>::make_table();

	std::cout << tab.m_table->items.size();
	std::cout << std::endl;

	//tab.removeBucket(3);

	for (auto&& b : tab.m_table->items) {
		if (b)
			std::cout << "Valid -> ";
		else
			std::cout << "Invalid -> ";
	}

	std::cout << "." << std::endl;

	for (size_t x = 0; x < tab.m_table->items.size(); ++x) {
		auto b = tab.m_table->items[x]->s_key;
		auto a = tab.m_table->items[x]->s_value;
		std::cout << "Key: " << b
				  << "\nValue: " << a << "\n";
	}
}
#endif
