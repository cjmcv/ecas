#include "bmp_reader.hpp"

namespace ecas {
namespace util {

// TODO: grayscale
BmpReader::BmpReader(uint32_t width, uint32_t height, uint32_t channels) {
    if (channels != 3 && channels != 4)
        ECAS_LOGE("BmpReader only supports 3 or 4 channel inputs for now.\n");

    bmp_info_header_.width = width;
    bmp_info_header_.height = height;
    bmp_info_header_.bit_count = channels * 8;

    NormalizeHeader();
}

BmpReader::BmpReader(const char *fname) {
    std::ifstream inp{fname, std::ios_base::binary};
    if (!inp) {
        ECAS_LOGE("Unable to open the input image file: %s \n.", fname);
    }

    ReadHeaders(inp);
    if (bmp_info_header_.height < 0) {
        ECAS_LOGE("The program can treat only BmpReader images with the origin in the bottom left corner!");
    }

    // Jump to the pixel data location
    inp.seekg(file_header_.offset_data, inp.beg);

    // Some editors will put extra info in the image file, we only save the headers and the data.
    NormalizeHeader();

    // Here we check if we need to take into account row padding
    if (bmp_info_header_.width % 4 == 0) { // 32bit其channels为4，一定能被4整除；只有24的需要判断，因为24的channel为3
        inp.read((char *)data_.data(), data_.size());
    }
    else {
        std::vector<uint8_t> padding_row(new_stride_ - row_stride_);
        for (int y = 0; y < bmp_info_header_.height; ++y) {
            inp.read((char *)(data_.data() + row_stride_ * y), row_stride_);
            inp.read((char *)padding_row.data(), padding_row.size());
        }
    }
}

void BmpReader::Write(const char *fname) {
    std::ofstream of{fname, std::ios_base::binary};
    if (!of) {
        ECAS_LOGE("Unable to open the output image file: %s \n.", fname);
    }

    WriteHeaders(of);

    if (bmp_info_header_.width % 4 == 0) {
        of.write((const char *)data_.data(), data_.size());
    }
    else {
        std::vector<uint8_t> padding_row(new_stride_ - row_stride_);
        for (int y = 0; y < bmp_info_header_.height; ++y) {
            of.write((const char *)(data_.data() + row_stride_ * y), row_stride_);
            of.write((const char *)padding_row.data(), padding_row.size());
        }
    }
}

void BmpReader::NormalizeHeader() {
    row_stride_ = bmp_info_header_.width * bmp_info_header_.bit_count / 8;
    if (bmp_info_header_.bit_count == 32) {
        bmp_info_header_.size = sizeof(BmpInfoHeader) + sizeof(BmpColorHeader);
        file_header_.offset_data = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + sizeof(BmpColorHeader);

        bmp_info_header_.compression = 3;
        file_header_.file_size = file_header_.offset_data + data_.size();
    }
    else if (bmp_info_header_.bit_count == 24) {
        bmp_info_header_.size = sizeof(BmpInfoHeader);
        file_header_.offset_data = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);

        bmp_info_header_.compression = 0;
        new_stride_ = row_stride_;
        while (new_stride_ % 4 != 0) {
            new_stride_++;
        }
        file_header_.file_size = file_header_.offset_data + data_.size() + bmp_info_header_.height * (new_stride_ - row_stride_);
    }
    data_.resize(bmp_info_header_.width * bmp_info_header_.height * bmp_info_header_.bit_count / 8);
}

void BmpReader::ReadHeaders(std::ifstream &inf) {
    inf.read((char *)&file_header_, sizeof(file_header_));
    if (file_header_.file_type != 0x4D42) {
        ECAS_LOGE("Error! Unrecognized file format.");
    }
    inf.read((char *)&bmp_info_header_, sizeof(bmp_info_header_));

    // The BmpColorHeader is used only for transparent images
    if (bmp_info_header_.bit_count == 32) {
        // Check if the file has bit mask color information
        if (bmp_info_header_.size >= (sizeof(BmpInfoHeader) + sizeof(BmpColorHeader))) {
            inf.read((char *)&bmp_color_header_, sizeof(bmp_color_header_));
            // Check if the pixel data is stored as BGRA and if the color space type is sRGB
            CheckColorHeader(bmp_color_header_);
        }
        else {
            ECAS_LOGE("Error! Unrecognized file format: This file does not seem to contain bit mask information.");
        }
    }
}

void BmpReader::WriteHeaders(std::ofstream &of) {
    of.write((const char *)&file_header_, sizeof(file_header_));
    of.write((const char *)&bmp_info_header_, sizeof(bmp_info_header_));
    if (bmp_info_header_.bit_count == 32) {
        of.write((const char *)&bmp_color_header_, sizeof(bmp_color_header_));
    }
}

// Check if the pixel data is stored as BGRA and if the color space type is sRGB
void BmpReader::CheckColorHeader(BmpColorHeader &bmp_color_header) {
    BmpColorHeader expected_color_header;
    if (expected_color_header.red_mask != bmp_color_header.red_mask ||
        expected_color_header.blue_mask != bmp_color_header.blue_mask ||
        expected_color_header.green_mask != bmp_color_header.green_mask ||
        expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
        ECAS_LOGE("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
    }
    if (expected_color_header.color_space_type != bmp_color_header.color_space_type) {
        ECAS_LOGE("Unexpected color space type! The program expects sRGB values");
    }
}

} // namespace util
} // namespace ecas