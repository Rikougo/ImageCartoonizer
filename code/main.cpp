#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Canny.hpp>
#include <LPE.hpp>
#include <featureExtraction.hpp>
#include <Texture.hpp>

int main(int argc, char * argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input_image> <output_image(png only)> [border-type(0-5)]" << std::endl;
        return 1;
    }

    std::filesystem::path l_path(argv[1]);
    std::filesystem::path l_outputPath(argv[2]);

    if (l_outputPath.extension() != ".png") {
        std::cerr << "Output file must be a png file, it has automatically been changed." << std::endl;
        l_outputPath.replace_extension(".png");
    }

    auto l_clock = std::chrono::high_resolution_clock::now();

    auto l_borderType = ImgCartoonizer::BorderType_Black;
    if (argc > 3) {
        int l_intValue = std::stoi(argv[3]);
        if (l_intValue < 0 || l_intValue > (int)ImgCartoonizer::BorderType_MoreAverage)
            throw std::runtime_error("Invalid border type");

        l_borderType = (ImgCartoonizer::BorderType)l_intValue;
    }

    auto l_image = ImgCartoonizer::Image::Load(l_path);
    auto zones = ImgCartoonizer::LPE(l_image,25);
    auto features = ImgCartoonizer::Feature::extractFeatures(l_image, zones);
    auto resImgFeat = ImgCartoonizer::Feature::generateFromFeatures(features, zones, l_image.width, l_image.height);

    ImgCartoonizer::Feature::setBorders(resImgFeat, features, zones, l_image.width, l_image.height, l_borderType, 0.3);
    ImgCartoonizer::Image::Save(l_outputPath, resImgFeat);

    auto l_endClock = std::chrono::high_resolution_clock::now();

    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(l_endClock - l_clock).count() << "ms" << std::endl;

    return 0;
}
