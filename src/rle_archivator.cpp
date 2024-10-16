#include "rle_archivator.h"

void RleArchivator::compress(const std::filesystem::path& input_file_path,
                             const std::filesystem::path& output_file_path) {
    auto [input_file, output_file] =
        CheckFiles(input_file_path, output_file_path);

    char current_char;
    char prev_char;
    uint32_t count = 0;

    if (input_file.get(prev_char)) {
        count = 1;
    } else {
        throw std::runtime_error("Input file is empty");
    }

    while (input_file.get(current_char)) {
        if (current_char == prev_char) {
            if (count < UINT32_MAX) {
                count++;
            } else {
                output_file.write(reinterpret_cast<char*>(&count),
                                  sizeof(count));
                output_file.put(prev_char);
                count = 1;
            }
        } else {
            output_file.write(reinterpret_cast<char*>(&count), sizeof(count));
            output_file.put(prev_char);
            prev_char = current_char;
            count = 1;
        }
    }

    output_file.write(reinterpret_cast<char*>(&count), sizeof(count));
    output_file.put(prev_char);

    input_file.close();
    output_file.close();
}

void RleArchivator::decompress(const std::filesystem::path& input_file_path,
                               const std::filesystem::path& output_file_path) {
    auto [input_file, output_file] =
        CheckFiles(input_file_path, output_file_path);

    uint32_t count;
    char character;

    while (input_file.read(reinterpret_cast<char*>(&count), sizeof(count))) {
        if (!input_file.get(character)) {
            throw std::runtime_error("Incorrect file");
        }

        for (uint32_t i = 0; i < count; ++i) {
            output_file.put(character);
        }
    }

    input_file.close();
    output_file.close();
}
