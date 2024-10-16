#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "archivator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Использование: " << argv[0]
                  << " <входной файл> <выходной файл>\n";
        return 1;
    }

    std::filesystem::path inputFile = argv[1];
    std::string outputFile = argv[2];

    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Не удалось открыть входной файл.\n";
        return 1;
    }

    // Заменить на реализацию архиватора
    std::unique_ptr<Archivator> archivator = std::make_unique<Archivator>();

    archivator->compress(inputFile, outputFile);

    std::cout << "Архивация завершена.\n" << std::endl;

    return 0;
}
