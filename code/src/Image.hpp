//
// Created by sakeiru on 11/7/2022.
//

#ifndef IMAGECARTOONIZER_IMAGE_HPP
#define IMAGECARTOONIZER_IMAGE_HPP

#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

namespace ImgCartoonizer {
    struct Image {
        std::vector<float> data;
        int width, height;
        int channels;

        Image() {
            this->width = -1;
            this->height = -1;
            this->channels = -1;
        }

        Image(const Image& p_source) {
            this->data.resize(p_source.data.size());
            std::copy(p_source.data.begin(), p_source.data.end(), this->data.begin());
            this->width = p_source.width; this->height = p_source.height; this->channels = p_source.channels;
        }

        [[nodiscard]] float * PixelAt(int x, int y) {
            return data.data() + (y * width + x) * channels;
        }

        [[nodiscard]] const float * PixelAt(int x, int y) const {
            return data.data() + (y * width + x) * channels;
        }

        bool Save(std::filesystem::path const &p_path) const {
            return Save(p_path, *this);
        }

        static Image Load(std::filesystem::path const &p_path) {
            Image l_result{};

            unsigned char * tmpData = stbi_load(
                    static_cast<const char *>(p_path.string().c_str()),
                    &l_result.width,
                    &l_result.height,
                    &l_result.channels, 0);

            l_result.data = std::vector<float>(l_result.width * l_result.height * l_result.channels);

            for(int i = 0 ; i < l_result.width*l_result.height*l_result.channels ; i ++){
                l_result.data[i] = tmpData[i]/255.0;
            }

            delete tmpData;

            return l_result;
        }

        static bool Save(std::filesystem::path const &p_path, Image const &p_image, float min = 0., float max = 1.0) {

           unsigned char * tmpData = new unsigned char[p_image.width * p_image.height * p_image.channels];

            for(int i = 0 ; i < p_image.width * p_image.height * p_image.channels ; i ++){
                tmpData[i] =  std::max(std::min((int)((p_image.data[i] - min ) / (max-min) * 255),255),0);
            }



            if (p_path.extension() == ".png") {
                return stbi_write_png(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        tmpData,
                        0
                        ) == 1;
            } else if (p_path.extension() == ".jpg") {
                return stbi_write_jpg(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        tmpData,
                        0
                        ) == 1;
            } /*else {
                throw std::exception("Unsupported file extension");
            }*/

            delete[] tmpData;
            return false;
        }
    };
}

#endif //IMAGECARTOONIZER_IMAGE_HPP
