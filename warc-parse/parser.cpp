#include "parser.h"
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>

int main(int argc, char** argv)
{
	if (argc < 1) {
		std::cerr << "Cant be called without a warc file as first parameter!";
		std::exit(1);
	}

	std::cout << "Processing file '" << argv[1] << "'. Be patient. :^)\n\n";
	std::cout << "TRIM: " << FieldTrim<std::string>("    Some   .  ")
			  << "\n";
	std::unique_ptr<std::ifstream> file = std::make_unique<std::ifstream>(std::string(argv[1]));
	std::istream* is = file.get();
	if (!is->good() && !file->good()) {
		std::cout << "Couldn't open " << argv[1] << ": "
				  << std::strerror(errno);
		std::exit(1);
	}

	std::string ver {};
	Field_Map MAPP;

	/*ReadVersion(*is, ver);
	ReadSubsequentFields(*is, MAPP);
	std::cout << "Version: "
			  << ver
			  << " \nKeyStore: \n";

	auto DisplayKV = [](std::pair<std::string, std::string> const& KV) {
		std::cout << "KEY: '" << KV.first << "' VALUE: '" << KV.second << "' \n";
	};

	for_each(MAPP, DisplayKV);
	*/

	while (is->good()) {
		auto Thus = ReadRecords(*is);
		if (!holds(Thus)) {
			std::cout << "No valid record.."
					  << "\n";
			continue;
		}

		std::string content = std::get<WarcRecord>(Thus).content();
		std::cout << std::string(2, '\n')
				  << std::string(20, '=')
				  << "\nContent-Len: " << content.length() << "\n"
				  << std::string(20, '=') << std::string(2, '\n')
				  << content;
		std::cout << "\n";
	}

	std::cout << std::endl;

	return 0;
}
