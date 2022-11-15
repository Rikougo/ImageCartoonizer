/*
#include <Image.hpp>
#include <Convolution.hpp>
*/

float map(float x, float inMin, float inMax, float outMin, float outMax){
    return (x-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
}

float bilinearInterpolation(float x,float y,float val1,float val2,float val3,float val4){
    float m1Val1 = map(x,-1,1,val1,val2);
    float m1Val2 = map(x,-1,1,val3,val4);
    return map(y, -1, 1, m1Val1, m1Val2);
}

namespace ImgCartoonizer {

    Image gradient(Image l_image){
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

        auto blurFilter = ImgCartoonizer::Filter::create(5,blurConv,1.0/159);
        auto xGrad = ImgCartoonizer::Filter::create(3,xgradConv);
        auto yGrad = ImgCartoonizer::Filter::create(3,ygradConv);

        auto greyBlury = ImgCartoonizer::filter(l_image, blurFilter).avgGreyScale();

        auto bluryxGrad = ImgCartoonizer::filter(greyBlury, xGrad);
        auto bluryyGrad = ImgCartoonizer::filter(greyBlury, yGrad);

        auto gradient = ImgCartoonizer::Image::Create(l_image.width,l_image.height,2);

        for(int i = 0 ; i < l_image.width * l_image.height ; i ++){
            gradient.data[i*2 + 0] = std::sqrt(bluryxGrad.data[i]*bluryxGrad.data[i] + bluryyGrad.data[i]*bluryyGrad.data[i]);
            gradient.data[i*2 + 1] = std::atan(bluryxGrad.data[i]/bluryyGrad.data[i]);
        }

        return gradient;
    }

    Image contour(Image l_image){

        auto gradient = ImgCartoonizer::gradient(l_image);
        auto contour = ImgCartoonizer::Image::Create(l_image.width,l_image.height,1);

        for(int y = 1 ; y < gradient.height-1 ; y++){
            for(int x = 1 ; x < gradient.width-1 ; x++){
                float val = gradient.data[(y*gradient.width+x)*2+0];

                float m1PosX = std::cos(gradient.data[(y*gradient.width+x)*2+1]);
                float m1PosY = std::sin(gradient.data[(y*gradient.width+x)*2+1]);

                float m2PosX = -std::cos(gradient.data[(y*gradient.width+x)*2+1]);
                float m2PosY = -std::sin(gradient.data[(y*gradient.width+x)*2+1]);

                float m1val = bilinearInterpolation(m1PosX,m1PosY,
                    gradient.data[((y-1)*gradient.width+(x-1))*2+0],
                    gradient.data[((y-1)*gradient.width+(x+1))*2+0],
                    gradient.data[((y+1)*gradient.width+(x-1))*2+0],
                    gradient.data[((y+1)*gradient.width+(x+1))*2+0]);

                float m2val = bilinearInterpolation(m2PosX,m2PosY,
                    gradient.data[((y-1)*gradient.width+(x-1))*2+0],
                    gradient.data[((y-1)*gradient.width+(x+1))*2+0],
                    gradient.data[((y+1)*gradient.width+(x-1))*2+0],
                    gradient.data[((y+1)*gradient.width+(x+1))*2+0]);

                if(val>m1val && val>m2val){
                    contour.data[y*contour.width+x] = val;
                }else{
                    contour.data[y*contour.width+x] = 0.0;
                }
            }
        }
        return contour;
    }


    Image cannyFilter(Image l_image){

        auto ct = contour(l_image);
        ct = ct.seuilHysteresis(0.2, 0.6);

        return ct;
    }
}