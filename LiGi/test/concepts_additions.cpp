#include "../concepts_additions.h"
#include <iostream>

void test(concepts::EqualKeyValue auto const& s)
{
	for (auto&& buck : s) {
		std::cout << "EqualKeyBuck -> Key: '" << buck.first << "', second: " << buck.second << "\n";
	}
	return;
}

template<concepts::NotEqualKeyValue T>
void test(T const& s)
{
	for (auto&& buck : s) {
		std::cout << "NotEqualBuck -> Key: '" << buck.first << "', second: " << buck.second << "\n";
	}
	return;
}

int main(int argc, char** argv)
{
	std::unordered_map<std::string, std::string> eqT {};
	eqT["first"] = "first";
	eqT["sec"] = "sec";
	eqT["3rd"] = "3rd";
	eqT["4th"] = "4th";
	std::unordered_map<int, std::string> diffT {};
	diffT[0] = "first";
	diffT[1] = "sec";
	diffT[2] = "3rd";
	diffT[3] = "4th";

	test(eqT);
	test(diffT);

	return 0;
}
