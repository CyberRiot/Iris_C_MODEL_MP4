#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include "media_reader.hpp"
#include <filesystem>

namespace fs = std::filesystem;

// Setters
void media_reader::set_chunk_size(uint32_t u_chunk_s) { chunk_size = u_chunk_s; }

void media_reader::set_chunk_type(const std::string &u_chunk_t) { chunk_type = u_chunk_t; }

void media_reader::set_sub_type(const std::string &u_sub_type) { sub_type = u_sub_type; }

void media_reader::set_additional_info(uint32_t u_additional_info_t) { additional_info = u_additional_info_t; }

void media_reader::set_major_brand(const std::string &s_maj_brand) { major_brand = s_maj_brand; }

void media_reader::set_minor_version(uint32_t u_minor_brand_t) { minor_version = u_minor_brand_t; }

void media_reader::set_compatible_brands(const std::vector<std::string> &vs_compatible_brands) { compatible_brands = vs_compatible_brands; }

void media_reader::set_creation_time(uint32_t u_creation_time_t) { creation_time = u_creation_time_t; }

void media_reader::set_modification_time(uint32_t u_modification_time_t) { modification_time = u_modification_time_t; }

void media_reader::set_timescale(uint32_t u_timescale_t) { timescale = u_timescale_t; }

void media_reader::set_duration(uint32_t u_duration_t) { duration = u_duration_t; }

// Getters
uint32_t media_reader::get_chunk_size() const { return chunk_size; }

std::string media_reader::get_chunk_type() const { return chunk_type; }

std::string media_reader::get_sub_type() const { return sub_type; }

uint32_t media_reader::get_additional_info() const { return additional_info; }

std::string media_reader::get_major_brand() const { return major_brand; }

uint32_t media_reader::get_minor_version() const { return minor_version; }

std::vector<std::string> media_reader::get_compatible_brands() const { return compatible_brands; }

uint32_t media_reader::get_creation_time() const { return creation_time; }

uint32_t media_reader::get_modification_time() const { return modification_time; }

uint32_t media_reader::get_timescale() const { return timescale; }

uint32_t media_reader::get_duration() const { return duration; }

std::vector<media_reader::TrackInfo> media_reader::get_tracks() const { return tracks; }

// Function to convert 4 bytes to uint32_t
uint32_t media_reader::format(const unsigned char *bytes) { return (uint32_t)((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]); }

// Function to convert 4 bytes to string
std::string media_reader::format_string(const unsigned char *bytes) {
    char str[5];
    std::memcpy(str, bytes, 4);
    str[4] = '\0';
    return std::string(str);
}

// Function to read input file
void media_reader::read_input_file(const std::string &path) {
    unsigned char bytes[8];
    FILE *f = fopen(path.c_str(), "rb");
    if (f) {
        while (fread(bytes, 1, 8, f) == 8) { // Read 8 bytes for size and type
            uint32_t size = format(bytes);
            std::string type = format_string(bytes + 4);

            std::cout << "Box Type: " << type << ", Box Size: " << size << std::endl;

            set_chunk_size(size);
            set_chunk_type(type);
            if (type == "ftyp") {
                parse_ftyp_box(f, size - 8); // -8 for already read bytes
            } else if (type == "moov") {
                parse_moov_box(f, size - 8);
            } else if (type == "mdat") {
                parse_mdat_box(f, size - 8);
            } else {
                fseek(f, size - 8, SEEK_CUR); // Skip unknown box
            }
        }
        fclose(f);
    } else {
        std::cerr << "Failed to open file." << std::endl;
    }
}

void media_reader::parse_ftyp_box(FILE *f, uint32_t size) {
    std::vector<unsigned char> data(size);
    if (fread(data.data(), 1, size, f) == size){
        std::string local_major_brand = format_string(data.data());
        uint32_t local_minor_version = format(data.data() + 4);
        std::cout << "Parsing ftyp box" << std::endl;
        set_major_brand(local_major_brand);
        set_minor_version(local_minor_version);

        if (size > 8) {
            std::vector<std::string> local_compatible_brands;
            for (size_t i = 8; i + 4 <= size; i += 4) {
                std::string brand = format_string(data.data() + i);
                local_compatible_brands.push_back(brand);
            }
            set_compatible_brands(local_compatible_brands);
        }
    }
}

void media_reader::parse_moov_box(FILE *f, uint32_t size) {
    unsigned char bytes[8];
    uint32_t remaining_size = size;
    while (remaining_size > 8 && fread(bytes, 1, 8, f) == 8) {
        uint32_t box_size = format(bytes);
        std::string box_type = format_string(bytes + 4);
        std::cout << "Parsing moov box" << std::endl;
        set_sub_type(box_type);

        if (box_type == "mvhd") {
            std::cout << "mvhd detected." << std::endl;
            parse_mvhd_box(f, box_size - 8);
        } else if (box_type == "trak") {
            std::cout << "trak detected." << std::endl;
            parse_trak_box(f, box_size - 8);
        } else {
            fseek(f, box_size - 8, SEEK_CUR); // Skip unknown sub-box
        }
        remaining_size -= box_size;
    }
}

void media_reader::parse_mvhd_box(FILE *f, uint32_t size) {
    std::vector<unsigned char> data(size);
    std::cout << "Parsing MVHD" << std::endl;
    if (fread(data.data(), 1, size, f) == size) {
        uint32_t local_creation_time = format(data.data() + 4);
        uint32_t local_modification_time = format(data.data() + 8);
        uint32_t local_timescale = format(data.data() + 12);
        uint32_t local_duration = format(data.data() + 16);

        set_creation_time(local_creation_time);
        set_modification_time(local_modification_time);
        set_timescale(local_timescale);
        set_duration(local_duration);
    }
}

void media_reader::parse_trak_box(FILE *f, uint32_t size) {
    unsigned char bytes[8];
    uint32_t remaining_size = size;
    std::cout << "Parsing trak box" << std::endl;

    TrackInfo track;

    while (remaining_size > 8 && fread(bytes, 1, 8, f) == 8) {
        uint32_t box_size = format(bytes);
        std::string box_type = format_string(bytes + 4);

        if (box_type == "tkhd") {
            std::cout << "tkhd Detected." << std::endl;
            parse_tkhd_box(f, box_size - 8, track);
        } else if (box_type == "mdia") {
            std::cout << "mdia Detected." << std::endl;
            parse_mdia_box(f, box_size - 8, track);
        } else {
            fseek(f, box_size - 8, SEEK_CUR); // Skip unknown sub-box
        }
        remaining_size -= box_size;
    }

    tracks.push_back(track); // Add track info to tracks vector
}

void media_reader::parse_tkhd_box(FILE *f, uint32_t size, TrackInfo &track) {
    std::vector<unsigned char> data(size);
    std::cout << "Parsing tkhd" << std::endl;
    if (fread(data.data(), 1, size, f) == size) {
        track.creation_time = format(data.data() + 4);
        track.modification_time = format(data.data() + 8);
        track.track_id = format(data.data() + 12);
        track.duration = format(data.data() + 20);
    }
}

void media_reader::parse_mdat_box(FILE *f, uint32_t size) {
    fseek(f, size, SEEK_CUR); // Skip the content of the mdat box
}

void media_reader::parse_mdia_box(FILE *f, uint32_t size, TrackInfo &track) {
    unsigned char bytes[8];
    uint32_t remaining_size = size;
    std::cout << "Parsing mdia" << std::endl;

    while (remaining_size > 8 && fread(bytes, 1, 8, f) == 8) {
        uint32_t box_size = format(bytes);
        std::string box_type = format_string(bytes + 4);

        if (box_type == "mdhd") {
            std::cout << "mdhd detected." << std::endl;
            parse_mdhd_box(f, box_size - 8, track);
        } else if (box_type == "hdlr") {
            std::cout << "hdlr detected." << std::endl;
            parse_hdlr_box(f, box_size - 8, track);
        } else if (box_type == "minf") {
            std::cout << "minf detected." << std::endl;
            parse_minf_box(f, box_size - 8);
        } else {
            fseek(f, box_size - 8, SEEK_CUR); // Skip unknown sub-box
        }
        remaining_size -= box_size;
    }
}

void media_reader::parse_mdhd_box(FILE *f, uint32_t size, TrackInfo &track) {
    std::vector<unsigned char> data(size);
    std::cout << "parsing mdhd." << std::endl;
    if (fread(data.data(), 1, size, f) == size) {
        track.media_timescale = format(data.data() + 12);
        track.media_duration = format(data.data() + 16);
    }
}

void media_reader::parse_hdlr_box(FILE *f, uint32_t size, TrackInfo &track) {
    std::vector<unsigned char> data(size);
    std::cout << "parsing hdlr" << std::endl;
    if (fread(data.data(), 1, size, f) == size) {
        track.handler_type = format_string(data.data() + 8);
    }
}

void media_reader::parse_minf_box(FILE *f, uint32_t size) {
    unsigned char bytes[8];
    uint32_t remaining_size = size;
    std::cout << "parsing minf" << std::endl;

    while (remaining_size > 8 && fread(bytes, 1, 8, f) == 8) {
        uint32_t box_size = format(bytes);
        std::string box_type = format_string(bytes + 4);

        if (box_type == "stbl") {
            std::cout << "stbl detected." << std::endl;
            parse_stbl_box(f, box_size - 8, tracks.back());
        } else {
            fseek(f, box_size - 8, SEEK_CUR); // Skip unknown sub-box
        }
        remaining_size -= box_size;
    }
}

void media_reader::parse_stbl_box(FILE *f, uint32_t size, TrackInfo &track) {
    std::vector<unsigned char> data(size);
    std::cout << "parsing stbl" << std::endl;
    if (fread(data.data(), 1, size, f) == size) {
        unsigned char* ptr = data.data();
        while (ptr < data.data() + size) {
            uint32_t box_size = format(ptr);
            std::string box_type = format_string(ptr + 4);

            if (box_type == "stsd") { // Sample Description Box
                parse_stsd_box(ptr, box_size, track);
            } else if (box_type == "stts") { // Time-to-Sample Box
                parse_stts_box(ptr, box_size, track);
            } else if (box_type == "stsc") { // Sample-to-Chunk Box
                parse_stsc_box(ptr, box_size, track);
            } else if (box_type == "stco") { // Chunk Offset Box
                parse_stco_box(ptr, box_size, track);
            } else if (box_type == "stsz") { // Sample Size Box
                parse_stsz_box(ptr, box_size, track);
            }
            ptr += box_size;
        }
    }
}

// Parsing functions for sample tables
void media_reader::parse_stsd_box(const unsigned char *data, uint32_t size, TrackInfo &track) {
    std::cout << "parse stsd" << std::endl;
    // Process sample description data if needed
}

void media_reader::parse_stts_box(const unsigned char *data, uint32_t size, TrackInfo &track) {
    std::cout << "parse stts" << std::endl;
    uint32_t entry_count = format(data + 4);
    const unsigned char* ptr = data + 8;
    for (uint32_t i = 0; i < entry_count; ++i) {
        uint32_t sample_count = format(ptr + i * 8);
        uint32_t sample_delta = format(ptr + i * 8 + 4);
        // Store sample count and delta if needed
    }
}

void media_reader::parse_stsc_box(const unsigned char *data, uint32_t size, TrackInfo &track) {
    std::cout << "parse stsc" << std::endl;
    uint32_t entry_count = format(data + 4);
    const unsigned char* ptr = data + 8;
    for (uint32_t i = 0; i < entry_count; ++i) {
        uint32_t first_chunk = format(ptr + i * 12);
        uint32_t samples_per_chunk = format(ptr + i * 12 + 4);
        uint32_t sample_description_index = format(ptr + i * 12 + 8);
        // Store chunk mapping if needed
    }
}

void media_reader::parse_stco_box(const unsigned char *data, uint32_t size, TrackInfo &track) {
    uint32_t entry_count = format(data + 4);
    std::cout << "parsing stco" << std::endl;
    const unsigned char* ptr = data + 8;
    for (uint32_t i = 0; i < entry_count; ++i) {
        track.sample_offsets.push_back(format(ptr + i * 4));
    }
}

void media_reader::parse_stsz_box(const unsigned char *data, uint32_t size, TrackInfo &track) {
    uint32_t sample_size = format(data + 4);
    uint32_t sample_count = format(data + 8);
    std::cout << "parsing stsz" << std::endl;
    const unsigned char* ptr = data + 12;
    if (sample_size == 0) {
        for (uint32_t i = 0; i < sample_count; ++i) {
            track.sample_sizes.push_back(format(ptr + i * 4));
        }
    } else {
        // All samples are the same size
        track.sample_sizes.resize(sample_count, sample_size);
    }
}

// New function to extract and read samples
void media_reader::extract_samples(const std::string &path) {
    std::string output_dir = "C:\\Users\\cyber\\Desktop\\Idea\\data\\video\\frames";
    if (!fs::exists(output_dir)) {
        std::cout << "Creating frames directory at: " << output_dir << std::endl;
        fs::create_directories(output_dir);
    }

    std::cout << "Extracting samples from: " << path << std::endl;

    FILE *f = fopen(path.c_str(), "rb");
    if (f) {
        for (size_t track_index = 0; track_index < tracks.size(); ++track_index) {
            const auto &track = tracks[track_index];
            std::string track_dir = output_dir + "\\track_" + std::to_string(track_index);
            if (!fs::exists(track_dir)) {
                std::cout << "Creating track directory at: " << track_dir << std::endl;
                fs::create_directories(track_dir);
            }

            std::cout << "Processing track " << track_index << " with " << track.sample_sizes.size() << " samples." << std::endl;

            for (size_t i = 0; i < track.sample_sizes.size(); ++i) {
                std::string frame_filename = track_dir + "\\frame_" + std::to_string(i) + ".bin";
                std::cout << "Writing frame " << i << " to " << frame_filename << std::endl;
                read_sample_data(f, track.sample_offsets[i], track.sample_sizes[i], frame_filename);
            }
        }
        fclose(f);
    } else {
        std::cerr << "Failed to open file for reading samples: " << path << std::endl;
    }
}

// New function to read sample data from file
void media_reader::read_sample_data(FILE *f, uint32_t offset, uint32_t size, const std::string &output_filename) {
    fseek(f, offset, SEEK_SET);
    std::vector<unsigned char> sample_data(size);
    fread(sample_data.data(), 1, size, f);

    std::ofstream outfile(output_filename, std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(sample_data.data()), size);
        outfile.close();
        std::cout << "Successfully wrote " << size << " bytes to " << output_filename << std::endl;
    } else {
        std::cerr << "Failed to open output file: " << output_filename << std::endl;
    }
}
