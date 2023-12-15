#include <iostream>
#include "fileexplorer.hpp"

int main() {
    std::string directoryPath;
    std::cout << "Enter a directory path: ";
    std::cin >> directoryPath;

    ListDirectory(directoryPath);

    return 0;
}
