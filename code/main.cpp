#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Canny.hpp>

int main() {
    auto l_image = ImgCartoonizer::Image::Load("./images/asterix.png");
    auto canny = cannyFilter(l_image);


    ImgCartoonizer::Image::Save("results/cannyHister.png", canny,0,1); 


    return 0;
}
