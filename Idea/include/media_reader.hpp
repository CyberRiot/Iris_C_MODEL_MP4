#ifndef __MEDIA_READER_HPP
#define __MEDIA_READER_HPP

#include <vector>
#include <iostream>
#include <string>
#include <cstdint>
#include <filesystem>
#include <cstring>

class media_reader {
public:
    struct TrackInfo {
        uint32_t creation_time;
        uint32_t modification_time;
        uint32_t track_id;
        uint32_t duration;
        uint32_t media_timescale;
        uint32_t media_duration;
        std::string handler_type;
        std::vector<uint32_t> sample_sizes;
        std::vector<uint32_t> sample_offsets;
    };

    // Setters
    void set_chunk_size(uint32_t u_chunk_s);
    void set_chunk_type(const std::string &u_chunk_t);
    void set_sub_type(const std::string &u_sub_type);
    void set_additional_info(uint32_t u_additional_info_t);

    void set_major_brand(const std::string &s_maj_brand);
    void set_minor_version(uint32_t u_minor_brand_t);
    void set_compatible_brands(const std::vector<std::string> &vs_compatible_brands);
    void set_creation_time(uint32_t u_creation_time_t);
    void set_modification_time(uint32_t u_modification_time_t);
    void set_timescale(uint32_t u_timescale_t);
    void set_duration(uint32_t u_duration_t);

    // Getters
    uint32_t get_chunk_size() const;
    std::string get_chunk_type() const;
    std::string get_sub_type() const;
    uint32_t get_additional_info() const;
    std::string get_major_brand() const;
    uint32_t get_minor_version() const;
    std::vector<std::string> get_compatible_brands() const;
    uint32_t get_creation_time() const;
    uint32_t get_modification_time() const;
    uint32_t get_timescale() const;
    uint32_t get_duration() const;
    std::vector<TrackInfo> get_tracks() const;

    // Functions to perform media_reader operations
    void read_input_file(const std::string &path);
    void extract_samples(const std::string &path);
    void read_sample_data(FILE *f, uint32_t offset, uint32_t size, const std::string &output_filename);

    // New function to decode and save frames
    void decode_and_save_frame(const std::vector<unsigned char> &sample_data, const std::string &output_path, int frame_number);

private:
    uint32_t chunk_size;
    std::string chunk_type;
    std::string sub_type;
    uint32_t additional_info;

    std::string major_brand;
    uint32_t minor_version;
    std::vector<std::string> compatible_brands;
    uint32_t creation_time;
    uint32_t modification_time;
    uint32_t timescale;
    uint32_t duration;

    std::vector<TrackInfo> tracks;

    // Functions to convert bytes
    uint32_t format(const unsigned char *bytes);
    std::string format_string(const unsigned char *bytes);

    // Parsing functions
    void parse_ftyp_box(FILE *f, uint32_t size);
    void parse_moov_box(FILE *f, uint32_t size);
    void parse_mvhd_box(FILE *f, uint32_t size);
    void parse_trak_box(FILE *f, uint32_t size);
    void parse_tkhd_box(FILE *f, uint32_t size, TrackInfo &track);
    void parse_mdat_box(FILE *f, uint32_t size);
    void parse_mdia_box(FILE *f, uint32_t size, TrackInfo &track);
    void parse_mdhd_box(FILE *f, uint32_t size, TrackInfo &track);
    void parse_hdlr_box(FILE *f, uint32_t size, TrackInfo &track);
    void parse_minf_box(FILE *f, uint32_t size);
    void parse_stbl_box(FILE *f, uint32_t size, TrackInfo &track);

    // Parsing functions for sample tables
    void parse_stco_box(const unsigned char *data, uint32_t size, TrackInfo &track);
    void parse_stsz_box(const unsigned char *data, uint32_t size, TrackInfo &track);

    void parse_stsd_box(const unsigned char *data, uint32_t size, TrackInfo &track);
    void parse_stts_box(const unsigned char *data, uint32_t size, TrackInfo &track);
    void parse_stsc_box(const unsigned char *data, uint32_t size, TrackInfo &track);
};

#endif
