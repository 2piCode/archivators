#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "huffman_archivator.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Need to use: <input_file> <output_compress_file> "
                     "<output_decompress_file>"
                  << std::endl;
        return 1;
    }

    std::filesystem::path input_file = argv[1];
    std::filesystem::path output_compress_file = argv[2];
    std::filesystem::path output_decompress_file = argv[3];

    std::unique_ptr<Archivator> archivator =
        std::make_unique<HuffmanArchivator>();

    archivator->compress(input_file, output_compress_file);
    archivator->decompress(output_compress_file, output_decompress_file);

    std::cout << "Proccess completed" << std::endl;

    return 0;
}
