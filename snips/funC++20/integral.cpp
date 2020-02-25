#include <iostream>
#include <concepts>
#include <type_traits>
#include <cstddef>

template<typename N>
concept bool Integral()
{
	return std::is_integral<T>::value;
}


template<typename N>
requires Integral<N>
T gcd(T a, T b)
{
	if(b == 0) { return a; }
	else
	{
		return gcd(b, a%b);
	}
}

int main()
{
	std::cout << gcd(1, 2) << "\n";
}
