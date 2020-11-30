#ifndef LRU_H
#define LRU_H
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

template<typename T>
struct LRUNode {
public:
	LRUNode()
	{
	}

	void null_it()
	{
		static_cast<T*>(this)->set_next(nullptr);
	}
};

const size_t lru_default_size = 8 * 256;
template<typename key_t, typename val_t>
class LRU : public std::enable_shared_from_this<LRU<key_t, val_t>> {
protected:
	using key_value_t = std::pair<key_t, val_t>;
	using pair_iterator_t = typename std::list<key_value_t>::iterator;
	using Size = size_t;
	Size m_Capacity;

public:
	static std::shared_ptr<LRU> make_shared(Size p) { return std::make_shared<LRU>(LRU(p)); }
	std::shared_ptr<LRU> getLRU() { return this->shared_from_this(); }

	explicit LRU(Size max_pre = lru_default_size)
	{
		m_Capacity = max_pre;
		m_cached_items = std::list<key_value_t>(m_Capacity);
		m_cached_references = std::unordered_map<key_t, pair_iterator_t>(m_Capacity);
		m_cached_items.resize(m_Capacity);
		m_cached_references.reserve(m_Capacity);
	}

	void movable_put(const key_t& key, val_t val)
	{
		m_cached_items.push_front(std::move(key_value_t(key, std::move(val))));
		auto it = m_cached_references.find(key);
		if (it != m_cached_references.end()) {
			m_cached_items.erase(it->second);
			m_cached_references.erase(it);
		}
		m_cached_references[key] = m_cached_items.begin();

		if (m_cached_references.size() > m_Capacity) {
			auto last = m_cached_items.end();
			--last;
			m_cached_references.erase(last->first);
			m_cached_items.pop_back();
		}
	}

	inline void put(const key_t& key, const val_t& val)
	{
		movable_put(key, val);
	}

	const val_t& get(const key_t& key)
	{
		using namespace std::literals;
		auto it = m_cached_references.find(key);
		if (it == m_cached_references.end()) {
			throw std::range_error("Ther is no such key '"s + std::to_string(key) + "' in cache"s);
		} else {
			m_cached_items.splice(m_cached_items.begin(), m_cached_items, it->second);
			return it->second->second;
		}
	}

	inline const val_t& operator[](const key_t t)
	{
		return get(t);
	}

	Size size() const
	{
		return m_cached_items.size();
	}

	Size Capacity() const
	{
		return m_Capacity;
	}

private:
	std::list<key_value_t> m_cached_items;
	std::unordered_map<key_t, pair_iterator_t> m_cached_references;
};
}

TEST_CASE("LRU Capacity")
{
	size_t s = 10;
	auto m_ = Li::LRU<size_t, size_t>(s);
	CHECK(m_.Capacity() == 10);
}

TEST_CASE("LRU get & operator[]")
{
	int s = 4;
	auto m = Li::LRU<int, int>(static_cast<size_t>(s));
	for (int i = 0; i != s; ++i) {
		m.movable_put(i, i + 1);
	}
	for (int i = 0; i != s; ++i) {
		CHECK(m.get(i) == i + 1);
		CHECK(m[i] == i + 1);
	}
}
TEST_CASE("LRU Deletion")
{
	size_t s = 100;
	auto m = Li::LRU<size_t, size_t>(s);
	for (size_t i = 0; i != s; ++i) {
		m.put(i, i);
	}
	m[0];
	CHECK((m.size()) == s * 2); // standard containers often are double as big as they need..
}

TEST_CASE("LRU make shared")
{
	size_t s = 10;
	auto p = Li::LRU<int, int>::make_shared(s);
	CHECK(p);
}

TEST_CASE("LRU Shared from this/getLRU")
{
	size_t s = 10;
	auto F = Li::LRU<int, int>::make_shared(s);
	CHECK(F);

	std::shared_ptr<Li::LRU<int, int>> p = F->getLRU();
	CHECK(p);
	CHECK(p->Capacity() == s);
	CHECK(std::is_same_v<decltype(p->getLRU()), decltype(F)>);
}

TEST_CASE("LRU Bench")
{

	constexpr size_t bench_size = 1000000;
	auto start = []() {
		return std::chrono::high_resolution_clock::now();
	};

	auto stop = [start](auto begin) {
		auto y = start();
		return std::chrono::duration_cast<std::chrono::milliseconds>(y - begin).count();
	};

	auto cur = start();
	auto m = Li::LRU<size_t, std::string>(bench_size);
	auto end = stop(cur);
	auto msg = "\n" + std::to_string(end) + "ms construction time.\n";

	cur = start();
	for (size_t i = 0; i != bench_size; ++i) {
		m.movable_put(i, std::to_string(i)); // faster insertion through copy-ellision and moving value
	}
	end = stop(cur);
	msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " items insertion.\n";

	cur = start();
	for (size_t i = 0; i != bench_size; ++i) {
		m.get(i);
	}
	end = stop(cur);
	msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " linear items search.\n";

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<size_t> distrib(0, bench_size - 1);

	std::stack<size_t> RandomNums {};
	for (size_t i = 0; i != bench_size; ++i)
		RandomNums.emplace(distrib(gen));

	cur = start();

	while (true) {
		if (RandomNums.size() == 0) {
			break;
		}

		m.get(RandomNums.top());
		RandomNums.pop();
	}

	end = stop(cur);
	msg += std::to_string(end) + "ms measured " + std::to_string(bench_size) + " random items search.";

	MESSAGE(msg);
}
#endif // LRU_H
