#include <iostream>

#include <Image.hpp>
#include <Kmean.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto l_image = ImgCartoonizer::Image::Load("./images/renard_noel.png");

    ImgCartoonizer::Kmean(l_image, 4);

    l_image.Save("test.png");

    return 0;
}
