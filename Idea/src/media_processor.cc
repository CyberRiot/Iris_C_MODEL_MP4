#include "media_processor.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void media_processor::add_media(const media_reader &media) {
    media_files.push_back(media);
}

void media_processor::process_directory(const std::string &directory_path) {
    for (const auto &entry : fs::directory_iterator(directory_path)) {
        if (entry.path().extension() == ".mp4") {
            media_reader reader;
            reader.read_input_file(entry.path().string());
            add_media(reader);
            media_file_paths.push_back(entry.path().string());
        }
    }
}

void media_processor::extract_all_samples(const std::string &output_path) {
    for (size_t i = 0; i < media_files.size(); ++i) {
        std::cout << "Extracting samples for file: " << media_file_paths[i] << std::endl;
        media_files[i].extract_samples(media_file_paths[i]);
    }
}

int main() {
    media_processor processor;
    processor.process_directory("C:\\Users\\cyber\\Desktop\\Idea\\data\\video");
    processor.extract_all_samples("C:\\Users\\cyber\\Desktop\\Idea\\data\\video\\frames");
    return 0;
}
