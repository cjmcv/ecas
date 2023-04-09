#ifndef ECAS_UTIL_BMP_READER_HPP_
#define ECAS_UTIL_BMP_READER_HPP_

#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "logger.hpp"

namespace ecas {
namespace util {

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    uint32_t file_size{ 0 };               // Size of the file (in bytes)
    uint16_t reserved1{ 0 };               // <Not used here> Reserved, always 0
    uint16_t reserved2{ 0 };               // <Not used here> Reserved, always 0
    uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
};

struct BmpInfoHeader {
    uint32_t size{ 0 };                      // Size of this header (in bytes)
    int32_t width{ 0 };                      // width of bitmap in pixels
    int32_t height{ 0 };                     // width of bitmap in pixels
                                             //       (if positive, bottom-up, with origin in lower left corner)
                                             //       (if negative, top-down, with origin in upper left corner)
    uint16_t planes{ 1 };                    // <Not used here> No. of planes for the target device, this is always 1
    uint16_t bit_count{ 0 };                 // No. of bits per pixel
    uint32_t compression{ 0 };               // <Not used here> 0 or 3 - for uncompressed images.
    uint32_t size_image{ 0 };                // <Not used here> 0 - for uncompressed images
    int32_t x_pixels_per_meter{ 0 };         // <Not used here>
    int32_t y_pixels_per_meter{ 0 };         // <Not used here>
    uint32_t colors_used{ 0 };               // <Not used here> No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{ 0 };          // <Not used here> No. of colors used for displaying the bitmap. If 0 all colors are required
};

struct BmpColorHeader {
    uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
    uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
    uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
    uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
    uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
    uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};
#pragma pack(pop)

class BmpReader {
public:
    BmpReader(uint32_t width, uint32_t height, uint32_t channels);

    BmpReader(const char *fname);

    void Write(const char *fname);

    inline uint8_t *data() { return &data_[0]; };

private:
    void NormalizeHeader();

    void ReadHeaders(std::ifstream &inf);
    
    void WriteHeaders(std::ofstream &of);

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void CheckColorHeader(BmpColorHeader &bmp_color_header);

private:
    BmpFileHeader file_header_;
    BmpInfoHeader bmp_info_header_;
    BmpColorHeader bmp_color_header_;
    std::vector<uint8_t> data_;

    uint32_t row_stride_;
    uint32_t new_stride_;
};

} // namespace util
} // namespace ecas

#endif // ECAS_UTIL_BMP_READER_HPP_