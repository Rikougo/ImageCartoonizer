//
// Created by sakeiru on 11/7/2022.
//

#ifndef IMAGECARTOONIZER_IMAGE_HPP
#define IMAGECARTOONIZER_IMAGE_HPP

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

namespace ImgCartoonizer {
    struct Image {
        unsigned char* data;
        int width, height;
        int channels;

        [[nodiscard]] unsigned char* PixelAt(int x, int y) const {
            return &data[(y * width + x) * channels];
        }

        bool Save(std::filesystem::path const &p_path) const {
            return Save(p_path, *this);
        }

        static Image Load(std::filesystem::path const &p_path) {
            Image l_result{};

            l_result.data = stbi_load(
                    static_cast<const char *>(p_path.string().c_str()),
                    &l_result.width,
                    &l_result.height,
                    &l_result.channels, 0);

            return l_result;
        }

        static bool Save(std::filesystem::path const &p_path, Image const &p_image) {
            if (p_path.extension() == "png") {
                return stbi_write_png(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        p_image.data,
                        0
                        ) == 0;
            } else if (p_path.extension() == "jpg") {
                return stbi_write_jpg(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        p_image.data,
                        0
                        ) == 0;
            } else {
                throw std::exception("Unsupported file extension");
            }
        }
    };
}

#endif //IMAGECARTOONIZER_IMAGE_HPP
