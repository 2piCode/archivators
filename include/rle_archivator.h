#ifndef RLK_ARCHIVATOR_H
#define RLK_ARCHIVator_H

#include "archivator.h"

class RleArchivator : public Archivator {
    void compress(const std::filesystem::path& input_file_path,
                          const std::filesystem::path& output_file_path);
    void decompress(const std::filesystem::path& input_file_path,
                            const std::filesystem::path& output_file_path);
};

#endif
