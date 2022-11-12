//
// Created by sakeiru on 11/7/2022.
//

#ifndef IMAGECARTOONIZER_IMAGE_HPP
#define IMAGECARTOONIZER_IMAGE_HPP

#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stbi_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

namespace ImgCartoonizer {
    struct Image {
        std::vector<float> data;
        int width, height;
        int channels;

        [[nodiscard]] const float * PixelAt(int x, int y) const {
            return data.data() + (y * width + x) * channels;
        }

        [[nodiscard]] const float * PixelAt(std::pair<int,int> p) const {
            return data.data() + (p.second * width + p.first) * channels;
        }

        bool Save(std::filesystem::path const &p_path) const {
            return Save(p_path, *this);
        }

        static Image Create(int sizeX, int sizeY, int channels){
           Image res{};

           res.data = std::vector<float>(sizeX*sizeY*channels);
           res.width  = sizeX;
           res.height = sizeY; 
           res.channels = channels;

           return res;
        }

        static Image Load(std::filesystem::path const &p_path) {
            Image l_result{};

            unsigned char * tmpData = stbi_load(
                    static_cast<const char *>(p_path.string().c_str()),
                    &l_result.width,
                    &l_result.height,
                    &l_result.channels, 0);

            l_result.data = std::vector<float>(l_result.width * l_result.height * l_result.channels);

            for(int i = 0 ; i < l_result.width*l_result.height*l_result.channels ; i ++){
                l_result.data[i] = tmpData[i]/255.0;
            }

            delete tmpData;

            return l_result;
        }
        
        [[nodiscard]] Image avgGreyScale(){
            auto res = Create(this->width, this->height, 1);
            
            for(int i = 0 ; i < this->width * this->height ; i ++){
                res.data[i] = 0.0;
                for(int c = 0 ; c < this->channels ; c++){
                    res.data[i] += this->data[i*this->channels+c];
                }
                res.data[i] /= this->channels;
            }
            return res;
        }

        [[nodiscard]] Image seuil(float seuil, float min=0., float max=1.0){
            auto res = Create(this->width, this->height, this->channels);
            
            for(int i = 0 ; i < this->width * this->height*this->channels ; i ++){
                res.data[i] = this->data[i] < seuil ? min : max;
            }
            return res;
        }

        [[nodiscard]] Image seuilHysteresis(float seuilLow, float seuilHigh, float min=0., float max=1.){
            auto res = Create(this->width, this->height, this->channels);

            auto aSeuiller = std::vector<std::pair<int,int>>();
            auto tag = std::vector<int>(this->width*this->height,0);

            /*
            0 on sait pas
            1 c'est haut
            2 c'est bas
            */

            for(int y = 0 ; y<this->height ; y ++){
                for(int x = 0 ; x < this->width ; x++){
                    aSeuiller.push_back(std::pair<int,int>(x,y));
                    //tag.push_back(0);
                }

            }

            bool looping = false;
            bool done = false;
            int nbRemaining = this->width * this->height;
            int pRemaining;
            while(!done && !looping){
                looping = pRemaining == nbRemaining;;
                pRemaining = nbRemaining;
                done = true;
                for(int i = 0 ; i < aSeuiller.size() ; i++){
                    int index = aSeuiller[i].second * this->width + aSeuiller[i].first;
                    if(tag[index] == 0){
                        float val = this->data[index];
                        if(val > seuilHigh){
                            res.data[index] = max;
                            nbRemaining--;
                            tag[index] = 1;
                        }else if(val < seuilLow){
                            res.data[index] = min;
                            nbRemaining--;
                            tag[index] = 2;
                        }else{
                            bool atLeastOneBig = false;
                            bool allSmall      = true;

                            for(int dy = -1 ; dy <= 1 ; dy++){
                                for(int dx = -1 ; dx <= 1 ; dx++){
                                    int ind = (aSeuiller[i].second+dy) * this->width + (aSeuiller[i].first+dx);
                                    if(aSeuiller[i].second+dy >= 0 && aSeuiller[i].second+dy < this->height &&
                                        aSeuiller[i].first+dx >= 0 && aSeuiller[i].first+dx  < this->width
                                    ){
                                        int tagVal = tag[(aSeuiller[i].second+dy) * this->width + (aSeuiller[i].first+dx)];
                                        if(tagVal==1){
                                            atLeastOneBig = true;
                                        }if(tagVal==2){
                                            allSmall = false;
                                        }
                                    }
                                }
                            }

                            if(allSmall){
                                res.data[index] = min;
                                nbRemaining--;
                                tag[index] = 2;
                            }else if(atLeastOneBig){
                                res.data[index] = max;
                                nbRemaining--;
                                tag[index] = 1;
                            }else{
                                done = false;
                            }
                        }
                    }
                }
                
            }

            for(int i = 0 ; i < this->width * this->height ; i ++ ){
                if(tag[i] == 0){
                    res.data[i] = 0.0;
                }
            }


            return res;
        }



        static bool Save(std::filesystem::path const &p_path, Image const &p_image, float min = 0., float max = 1.0) {

           unsigned char * tmpData = new unsigned char[p_image.width * p_image.height * p_image.channels];

            for(int i = 0 ; i < p_image.width * p_image.height * p_image.channels ; i ++){
                tmpData[i] =  std::max(std::min((int)((p_image.data[i] - min ) / (max-min) * 255),255),0);
            }



            if (p_path.extension() == ".png") {
                return stbi_write_png(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        tmpData,
                        0
                        ) == 1;
            } else if (p_path.extension() == ".jpg") {
                return stbi_write_jpg(
                        static_cast<const char*>(p_path.string().c_str()),
                        p_image.width,
                        p_image.height,
                        p_image.channels,
                        tmpData,
                        0
                        ) == 1;
            } /*else {
                throw std::exception("Unsupported file extension");
            }*/

            delete tmpData;
            return 0;
        }
    };
}

#endif //IMAGECARTOONIZER_IMAGE_HPP
