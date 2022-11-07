#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto l_image = ImgCartoonizer::Image::Load("./images/asterix.png");

    float blurConv[] = {
        2, 4 , 5 , 4 , 2, 
        4, 9 , 12, 9 , 4,
        5, 12, 15, 12, 5,
        4, 9 , 12, 9 , 4,
        2, 4 , 5 , 4 , 2, 
    };

    float xgradConv[] = {
         -1,0,1,
        -2,0,2,
         -1,0,1,        
    };

    float ygradConv[] = {
        -1,-2,-1,
        0,0,0,
        1,2,1,        
    };

    auto blur = ImgCartoonizer::Filter::create(5,blurConv,1.0/159);
    auto xGrad = ImgCartoonizer::Filter::create(3,xgradConv);
    auto yGrad = ImgCartoonizer::Filter::create(3,ygradConv);

    auto blury = ImgCartoonizer::filter(l_image, blur);

    auto bluryxGrad = ImgCartoonizer::filter(blury, xGrad);
    auto bluryyGrad = ImgCartoonizer::filter(blury, yGrad);

    ImgCartoonizer::Image::Save("asterixGradX1.png", bluryxGrad,-1,1);
    ImgCartoonizer::Image::Save("asterixGradY.png", bluryyGrad);


    return 0;
}
