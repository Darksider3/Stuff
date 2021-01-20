#include "Poco/DigestStream.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/MD5Engine.h"

#include <iostream>
#include <vector>

int exitHelp(char** args)
{
    std::cout << args[0] << " file1 file2 file3...." << std::endl;
    return EXIT_FAILURE;
}

void PrintMD5(const std::string& in)
{
    Poco::File iFile { in };
    Poco::MD5Engine md5;
    Poco::DigestOutputStream ds(md5);
    if (!iFile.exists() || !iFile.canRead()) {
        std::cout << "File: " << in << " could not be found or read.\n";
    }

    Poco::FileInputStream fiS { iFile.path() };
    std::string read;
    fiS >> read;
    ds << read;
    std::cout << iFile.path() << ": " << Poco::DigestEngine::digestToHex(md5.digest()) << "\n";
    return;
}
int main(int argc, char** argv)
{

    if (argc < 2) {
        return exitHelp(argv);
    }

    // split argv based on range(ignore [0](which is our binaries path)
    std::vector<std::string> SplitInput { argv + 1, argv + argc };
    std::for_each(SplitInput.begin(), SplitInput.end(), PrintMD5);

    /*
    ds << "abcdefghijklmnopqrstuvwxyz";
    ds.close();
    std::cout << Poco::DigestEngine::digestToHex(md5.digest()) << std::endl;*/

    std::flush(std::cout);
    return 0;
}
