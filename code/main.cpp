#include <iostream>

#include <Image.hpp>
#include <Convolution.hpp>
#include <Canny.hpp>
#include <LPE.hpp>
#include <featureExtraction.hpp>
#include <Texture.hpp>

int main() {
    auto l_image = ImgCartoonizer::Image::Load("./images/noah.png");
    //auto canny = cannyFilter(l_image);

    auto zones = LPE(l_image,25);

    auto features = ImgCartoonizer::Feature::extractFeatures(l_image, zones);
    auto textures = ImgCartoonizer::Texture::extractTextures(l_image, zones);

    auto resImgTex  = ImgCartoonizer::Texture::generateFromTextures(textures, zones, l_image.width, l_image.height);
    auto resImgFeat = ImgCartoonizer::Feature::generateFromFeatures(features, zones, l_image.width, l_image.height);

    ImgCartoonizer::Feature::setBorders(resImgTex,  features, zones, l_image.width, l_image.height, 0, 0.3);
    ImgCartoonizer::Feature::setBorders(resImgFeat, features, zones, l_image.width, l_image.height, 0, 0.3);

    ImgCartoonizer::Image::Save("results/noahTextures.png", resImgTex);
    ImgCartoonizer::Image::Save("results/noahFlat.png", resImgFeat);

    



    
    {
        /*
        auto features = ImgCartoonizer::extractFeatures(l_image, zones);


        ImgCartoonizer::Image::Save("results/MathieuBlack.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 0),0,1);
        ImgCartoonizer::Image::Save("results/MathieuWhite.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 1),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvg.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 2),0,1);
        ImgCartoonizer::Image::Save("results/MathieuInvAvg.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 3),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins10.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,0.1),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus10.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5, 0.1),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins20.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.2),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus20.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.2),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins30.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.3),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus30.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.3),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins40.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.4),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus40.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.4),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins50.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.5),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus50.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.5),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins60.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.6),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus60.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.6),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins70.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.7),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus70.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.7),0,1);
        
        ImgCartoonizer::Image::Save("results/MathieuAvgMoins80.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.8),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus80.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.8),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins90.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,.9),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus90.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,.9),0,1);

        ImgCartoonizer::Image::Save("results/MathieuAvgMoins100.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 4,1.0),0,1);
        ImgCartoonizer::Image::Save("results/MathieuAvgPlus100.png", ImgCartoonizer::generateFromFeatures(features, zones, l_image.width, l_image.height, 5,  1.0),0,1);
        */
    }


    return 0;
}
