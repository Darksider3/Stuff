#include <iostream>
#include <iterator>
#include <algorithm>

class num_iterator
{
public:
	int i;
	explicit num_iterator(int pos = 0) : i{pos}
	{}
	int operator*() const { return i; }

	num_iterator& operator++()
	{
    i+=10;
		return *this;
	}
	bool operator!=(const num_iterator &other){return i != other.i;}
};


class num_range {
	int a, b;
public:
	num_range(int f, int to) : a{f}, b{to}
	{}

	num_iterator begin() const {return num_iterator{a};}
	num_iterator end() const {return num_iterator{b};}
};

int main()
{
	num_range r {100,300};
	for(int i : r)
	{
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	return 0;
}
