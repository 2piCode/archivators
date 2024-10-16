#ifndef FILE_STREAMS_H
#define FILE_STREAMS_H

#include <filesystem>
#include <fstream>

struct FileStreams {
    std::ifstream input_file_;
    std::ofstream output_file_;

    FileStreams(const std::filesystem::path& input_file,
                const std::filesystem::path& output_file)
        : input_file_(input_file, std::ios::binary),
          output_file_(output_file, std::ios::binary) {
        if (!input_file_) {
            throw std::invalid_argument("Can't open input file.");
        }
        if (!output_file_) {
            throw std::invalid_argument("Can't open output file.");
        }
    }
};

#endif
