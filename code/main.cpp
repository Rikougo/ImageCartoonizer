#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Canny.hpp>
#include <LPE.hpp>

int main()
{
    auto l_image = ImgCartoonizer::Image::Load("./images/asterix2.png");
    auto canny = cannyFilter(l_image);

    auto wota = LPE(l_image);

    ImgCartoonizer::Image::Save("results/wotaMathieu.png", wota, 0, 1);

    return 0;
}
