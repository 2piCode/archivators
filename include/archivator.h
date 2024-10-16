#ifndef ARCHIVATOR_H
#define ARCHIVATOR_H

#include <filesystem>

#include "file_streams.h"

class Archivator {
   public:
    virtual ~Archivator() = default;

    virtual void compress(const std::filesystem::path& input_file_path,
                          const std::filesystem::path& output_file_path) = 0;
    virtual void decompress(const std::filesystem::path& input_file_path,
                            const std::filesystem::path& output_file_path) = 0;

   protected:
    FileStreams CheckFiles(const std::filesystem::path& input_file,
                           const std::filesystem::path& output_file) const {
        return FileStreams(input_file, output_file);
    }
};

#endif
