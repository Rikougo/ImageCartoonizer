#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Kmean.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto l_image = ImgCartoonizer::Image::Load("./images/renard_noel.png");

    auto l_kmean = ImgCartoonizer::Kmean(l_image, 2, 15);

    l_kmean.Save("test.png");


    return 0;
}
