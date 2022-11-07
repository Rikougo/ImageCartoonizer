#include <iostream>

#include <Image.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto l_image = ImgCartoonizer::Image::LoadFromPath("./resources/test.png");

    return 0;
}
