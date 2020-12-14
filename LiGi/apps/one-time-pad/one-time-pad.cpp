#include <bsd/stdlib.h> // arc4_random
#include <climits>
#include <iostream>
#include <vector>

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

constexpr size_t cipherRegularBlock = 32;

class StreamCipher {
	void GenX(std::vector<unsigned char>& vec, ssize_t x = cipherRegularBlock)
	{
		for (short i = 0; i < x; ++i)
			vec.emplace_back(static_cast<unsigned char>(arc4random_uniform(UCHAR_MAX - 1)));
	}

public:
	std::vector<unsigned char> genCipher(size_t len)
	{
		std::vector<unsigned char> operating;
		size_t in_len = std::max<size_t>((len / 16), 1); // at least on operation with double key size
		for (size_t i = 0; i < in_len; ++i)
			GenX(operating);

		return operating;
	}

	std::string decipher(std::vector<unsigned char>& cipher, std::string_view thing)
	{
		return encipher(thing, cipher);
	}

	std::string encipher(std::string_view thing, std::vector<unsigned char>& cipher)
	{
		std::string Enciphered {};
		size_t len = thing.length();
		/*
		 * @TODO: Warn about insecure usage - cipher should be as long as the string or bigger!
		 */
		for (size_t i = 0; i < len; ++i) {
			Enciphered += (thing[i] ^ cipher[i]);
		}
		return Enciphered;
	}
};

int main()
{
	StreamCipher x {};
	std::string input;
	std::cout << "Please insert SOMETHING: ";
	std::getline(std::cin, input);

	auto cipher = x.genCipher(input.length());
	auto ciphered = x.encipher(input, cipher);
	std::cout << x.decipher(cipher, ciphered);

	std::cout << std::endl;
}
