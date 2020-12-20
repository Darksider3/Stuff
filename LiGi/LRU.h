#ifndef LRU_H
#define LRU_H
#include "Decls.h"
#include <list>
#include <memory>
#include <unordered_map>

#ifndef DOCTEST_CONFIG_DISABLE
#    include <chrono>
#    include <random>
#    include <stack>
#endif
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

namespace Li {

/* LRU cached_items
----> std::list.splice
1099ms construction time.
2667ms measured 30000000 items insertion.
445ms measured 30000000 linear items search.
6897ms measured 30000000 random items search.

----> std::list insert move iterator
1088ms construction time.
2721ms measured 30000000 items insertion.
188ms measured 30000000 linear items search.
2283ms measured 30000000 random items search.

----> std::deque insert move iterator

553ms construction time.
2627ms measured 30000000 items insertion.
546ms measured 30000000 linear items search.
4406ms measured 30000000 random items search.
 */

const Size lru_default_size = 8 * 256; ///< The default LRU size when no arguments are supplied

/**
 *  @brief LRU caching class
 *
 * An LRU Cache is, put simply, a "usage" Cache. It means "Least Recently Used" which means
 * it keeps track of the recently used items, and kicks, when stressing the capacity, least
 * one used, on insertion out of the cache.
 *
 * @author darksider3
 * @date 17.9.2016
 * @tparam key_t Key type
 * @tparam val_t Value Type
 */
template<typename key_t, typename val_t>
class LRU : public std::enable_shared_from_this<LRU<key_t, val_t>> {
private:
	using key_value_t = std::pair<key_t, val_t>;                       ///< Key-Pair of Key and Value supplied through template parameters
	using pair_iterator_t = typename std::list<key_value_t>::iterator; ///< key_value_t iterator
	Size m_Capacity;                                                   ///< Capacity of given LRU

public:
	/**
	 * @brief make_shared creates and instanciates an `std::shared_ptr<Li::LRU<key_t, val_t>>` for you.
	 * @param p Size of LRU
	 * @return std::shared_ptr<LRU> with given capacity
	 */
	static std::shared_ptr<LRU> make_shared(Size p = lru_default_size) { return std::make_shared<LRU>(LRU(p)); }

	/**
	 * @brief getLRU creates a new std::shared_ptr on an instance of a class. TAKE CARE: YOU NEED TO HAVE ONE ALREADY ON IT!
	 * @return std::shared_ptr<LRU> new std::shared_ptr pointing to this instance
	 */
	std::shared_ptr<LRU> getLRU() { return this->shared_from_this(); }

	/**
	 * @brief Instanciate LRU with given capacity
	 * @param max_pre Size of wanted LRU
	 */
	explicit LRU(Size max_pre = lru_default_size)
	{
		m_Capacity = max_pre;
		m_cached_items = std::list<key_value_t>(m_Capacity);
		m_cached_references = std::unordered_map<key_t, pair_iterator_t>(m_Capacity);
		m_cached_items.resize(m_Capacity);
		m_cached_references.reserve(m_Capacity);
	}

	/**
	 * @brief movable_put inserts `val` into the LRU with `key`, through moving `val`.
	 * @param key Key to place&lookup item
	 * @param val Value to store
	 */
	void movable_put(const key_t& key, val_t&& val)
	{
		auto it = m_cached_references.find(key);
		m_cached_items.push_front(key_value_t(key, std::move(val)));

		// if item is already in the list, remove it's old reference
		if (it != m_cached_references.end()) {
			m_cached_items.erase(it->second);
			m_cached_references.erase(it);
		}

		// if the new item exceeds the limit of the cache, erase the least recently used one
		if (m_cached_references.size() > m_Capacity) {
			auto last = m_cached_items.end();
			--last;
			m_cached_references.erase(last->first);
			m_cached_items.pop_back();
		}

		m_cached_references[key] = m_cached_items.begin();
	}

	/**
	 * @brief put see `movable_put` but without the moving part
	 * @param key Key to place&lookup item
	 * @param val Value to store
	 */
	inline void put(const key_t& key, val_t val)
	{
		movable_put(key, std::move(val));
	}

	/**
	 * @brief get Key from LRU, throws `std::range_error` in case it couldn't find it
	 * @param key Key to lookup
	 * @return val_t Value of `key`
	 * @throws std::range_error when key couldn't be found
	 */
	const val_t& get(const key_t& key)
	{
		using namespace std::literals;
		auto it = m_cached_references.find(key);
		if (it == m_cached_references.end()) {
			throw std::range_error("Ther is no such key '"s + std::to_string(key) + "' in cache"s);
		} else {
			// put the used item in front of the list
			m_cached_items.splice(m_cached_items.begin(), m_cached_items, it->second);

			/*m_cached_items.insert(m_cached_items.begin(),
				std::make_move_iterator(it->second),
				std::make_move_iterator(it->second));*/

			return it->second->second;
		}
	}

	/**
	 * @brief operator[] alias of `get(const key_t& val)`
	 * @param t Key to lookup
	 * @return `val_t` Value of `key`
	 */
	inline const val_t& operator[](const key_t t)
	{
		return get(t);
	}

	/**
	 * @brief has Determine if key is in Cache or not
	 * @param key Key to lookup
	 * @return bool true when found, otherwise false
	 */
	bool has(const key_t& key) const
	{
		return m_cached_references.find(key) != m_cached_references.end();
	}

	/**
	 * @brief is_at_beginning Helper to excerise if a given `key` is currently at position 0 of the Cache
	 * @param key Key to lookup
	 * @return bool True when key is first in Cache, false when not
	 */
	bool is_at_beginning(const key_t& key)
	{
		return m_cached_references.find(key)->first == m_cached_items.front().first;
	}

	/**
	 * @brief size Returns size of LRU
	 * @return `LRU::Size`
	 */
	Size size() const
	{
		return m_cached_items.size();
	}

	/**
	 * @brief Capacity of given LRU
	 * @return `LRU::Size`
	 */
	Size Capacity() const
	{
		return m_Capacity;
	}

	/**
	 * @brief Resize resizes the LRU. !!!WARNING!!! destructible! Also clears out everything first!
	 * @param cap Size to resize to
	 */
	void Resize(Size cap)
	{
		clear();
		m_cached_items.resize(cap);
		m_cached_references.reserve(cap);
		m_Capacity = cap;
	}

	/**
	 * @brief Clears out LRU
	 */
	void clear()
	{
		m_cached_items.clear();
		m_cached_references.clear();
	}

private:
	std::list<key_value_t> m_cached_items;                          ///< List holding the inserted values
	std::unordered_map<key_t, pair_iterator_t> m_cached_references; ///< Map holding references to iterators in the cache
};
}

TEST_SUITE("LRU")
{
	constexpr int small_case_size = 25;
	TEST_CASE("LRU exists")
	{
		Li::LRU<int, int> c(small_case_size);
		for (auto i = 0; i < small_case_size; ++i) {
			c.put(i, i + 1);
		}
		CHECK(c.has(23));
		CHECK(c.get(23) == 24);
		CHECK_FALSE(c.has(29));
	}

	TEST_CASE("LRU Range-Throw")
	{
		Li::LRU<int, int> c(small_case_size);
		CHECK_THROWS(c.get(small_case_size * 2));
	}

	TEST_CASE("LRU Capacity")
	{
		auto m_ = Li::LRU<Size, Size>(small_case_size);
		CHECK(m_.Capacity() == small_case_size);
	}

	TEST_CASE("LRU get & operator[]")
	{
		auto m = Li::LRU<int, int>(static_cast<Size>(small_case_size));
		for (int i = 0; i != small_case_size; ++i) {
			m.movable_put(i, i + 1);
		}
		for (int i = 0; i != small_case_size; ++i) {
			CHECK(m.get(i) == i + 1);
			CHECK(m[i] == i + 1);
		}

		m[0]; // reference to bring it up
		CHECK(m.is_at_beginning(0));
	}

	TEST_CASE("LRU Deletion")
	{
		auto m = Li::LRU<Size, Size>(small_case_size);
		for (Size i = 0; i != small_case_size; ++i) {
			m.put(i, i);
		}
		m[small_case_size - 1];
		CHECK((m.size()) == small_case_size * 2); // standard containers often are double as big as they need..
	}

	TEST_CASE("LRU make shared")
	{
		auto p = Li::LRU<int, int>::make_shared(small_case_size);
		CHECK(p);
	}

	TEST_CASE("LRU Shared from this/getLRU")
	{
		auto F = Li::LRU<int, int>::make_shared(small_case_size);
		CHECK(F);

		std::shared_ptr<Li::LRU<int, int>> p = F->getLRU();
		CHECK(p);
		CHECK(p->Capacity() == small_case_size);
		CHECK(std::is_same_v<decltype(p->getLRU()), decltype(F)>);
	}
}

TEST_SUITE("Benchmark" * doctest::skip())
{
	TEST_CASE("LRU Bench")
	{

		constexpr Size bench_size = 30000000;
		auto start = []() {
			return std::chrono::high_resolution_clock::now();
		};

		auto stop = [start](auto begin) {
			auto y = start();
			return std::chrono::duration_cast<std::chrono::milliseconds>(y - begin).count();
		};

		auto cur = start();
		auto m = Li::LRU<Size, std::string>(bench_size);
		auto end = stop(cur);
		auto msg = "\n" + std::to_string(end) + "ms construction time.\n";

		cur = start();
		for (Size i = 0; i != bench_size; ++i) {
			m.movable_put(i, std::to_string(i)); // faster insertion through copy-ellision and moving value
		}
		end = stop(cur);
		msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " items insertion.\n";

		cur = start();
		for (Size i = 0; i != bench_size; ++i) {
			m.get(i);
		}
		end = stop(cur);
		msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " linear items search.\n";

		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<Size> distrib(0, bench_size - 1);

		std::stack<Size> RandomNums {};
		for (Size i = 0; i != bench_size; ++i)
			RandomNums.emplace(distrib(gen));

		cur = start();

		while (true) {
			if (RandomNums.empty()) {
				break;
			}

			m.get(RandomNums.top());
			RandomNums.pop();
		}

		end = stop(cur);
		msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " random items search.";

		MESSAGE(msg);
	}
}
#endif // LRU_H
