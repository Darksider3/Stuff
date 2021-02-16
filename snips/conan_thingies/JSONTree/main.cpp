#include "common.hpp"
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    std::vector<fs::path> Pathes;
    auto RcrsvIterator = fs::recursive_directory_iterator(fs::current_path());
    for (auto& Element : RcrsvIterator) {
        Pathes.push_back(Element);
    }

    for (auto& Path : Pathes) {
        fmt::print(fmt::format("Path: {}\n", Path));
    }
}