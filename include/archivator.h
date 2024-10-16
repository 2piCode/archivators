#ifndef ARCHIVATOR_H
#define ARCHIVATOR_H

#include <filesystem>
#include <string>

class Archivator {
   public:
    virtual ~Archivator() = default;

    virtual void compress(const std::filesystem::path& inputFile,
                          const std::filesystem::path& outputFile) = 0;
    virtual void decompress(const std::filesystem::path& inputFile,
                            const std::filesystem::path& outputFile) = 0;
};

#endif
