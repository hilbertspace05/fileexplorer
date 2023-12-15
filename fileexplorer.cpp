#include <iostream>
#include <boost/filesystem.hpp>
#include "fileexplorer.hpp"

namespace fs = boost::filesystem;

void ListDirectory(const fs::path& directoryPath) {
    if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
        std::cout << "Contents of directory: " << directoryPath << std::endl;
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            std::cout << (fs::is_directory(entry) ? "[Dir] " : "[File] ") << entry.path().filename() << std::endl;
        }
    } else {
        std::cerr << "Directory not found: " << directoryPath << std::endl;
    }
}
