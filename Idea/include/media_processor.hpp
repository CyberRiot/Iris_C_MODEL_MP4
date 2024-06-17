#ifndef MEDIA_PROCESSOR_HPP
#define MEDIA_PROCESSOR_HPP

#include "media_reader.hpp"
#include <vector>
#include <string>
#include <filesystem>

class media_processor {
    std::vector<media_reader> media_files;
    std::vector<std::string> media_file_paths; // Store paths of media files

public:
    void add_media(const media_reader &media);
    void process_directory(const std::string &directory_path);
    void process_all_media();
    void extract_all_samples(const std::string &output_path);
};

#endif
