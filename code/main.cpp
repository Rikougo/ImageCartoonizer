#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Canny.hpp>

int main() {
    auto l_image = ImgCartoonizer::Image::Load("./images/asterix.png");
    auto canny = cannyFilter(l_image);


    ImgCartoonizer::Image::Save("results/canny.png", canny,-1,1);


    return 0;
}
