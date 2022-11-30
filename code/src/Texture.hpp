std::pair<float,float> operator+(const std::pair<float,float> &a, const std::pair<float,float> &b)
{
    return std::make_pair(a.first+b.first,a.second+b.second);
}

std::pair<float,float> operator-(const std::pair<float,float> &a, const std::pair<float,float> &b)
{
    return std::make_pair(a.first-b.first,a.second-b.second);
}

std::pair<float,float> operator*(const std::pair<float,float> &a, const float &b)
{
    return std::make_pair(a.first*b,a.second*b);
}

std::pair<float,float> operator*(const float &b,const std::pair<float,float> &a)
{
    return std::make_pair(a.first*b,a.second*b);
}

std::pair<float,float> operator/(const std::pair<float,float> &a, const float &b)
{
    float inv = 1.0/b;
    return std::make_pair(a.first*inv,a.second*inv);
}

std::pair<float,float> operator/(const float &b,const std::pair<float,float> &a)
{
    float inv = 1.0/b;
    return std::make_pair(a.first*inv,a.second*inv);
}

float sqlen(const std::pair<float,float> &a){
    return a.first*a.first + a.second*a.second;
}

float len(const std::pair<float,float> &a){
    return(sqrt(sqlen(a)));
}

std::pair<float,float> normalize(const std::pair<float,float> &a){
    return(a/len(a));
}


namespace ImgCartoonizer {  

    struct Imagette{
        int startX;
        int startY;

        int endX;
        int endY;

        int width;
        int height;

        std::vector<Color> pixels; // row-major
    };

    struct Texture{
        int startX;
        int startY;

        int sizeX;
        int sizeY;

        float density;
        float dotSize;

        Color dotColor;

        Color startBackgroundColor;
        Color endBackgroundColor;

        Color avgColor;

        ///////

        int c1x;
        int c1y;

        int c2x;
        int c2y;

        float direction;

        std::vector<std::pair<std::pair<int,int>,float>> dots;


        Texture(){
            startX = 0;
            startY = 0;

            sizeX = 100;
            sizeY = 100;

            density = 0.1;
            dotSize = 4;

            dotColor = Color(0,0,0);

            direction = 3.141592 / 8.0;

            startBackgroundColor = Color(1,0,0);
            endBackgroundColor   = Color(0,0,1);

            avgColor = Color(1,1,0);
        }

        void generate(){

            //std::cout<<"startX : "<<startX<<" startY : "<<startY<<" sizeX : "<<sizeX<<" sizeY : "<<sizeY<<std::endl;


            float midX = sizeX/2.0;
            float midY = sizeY/2.0;

            float maxDist = 0.5 * std::sqrt(sizeX*sizeX + sizeY*sizeY);

            c1x = midX - maxDist * std::cos(direction);
            c1y = midY - maxDist * std::sin(direction);

            c2x = midX + maxDist * std::cos(direction);
            c2y = midY + maxDist * std::sin(direction);

            
            if(c1x < 0) c1x = 0;
            if(c1x > sizeX) c1x = sizeX;

            if(c2x < 0) c2x = 0;
            if(c2x > sizeX) c2x = sizeX;

            if(c1y < 0) c1y = 0;
            if(c1y > sizeY) c1y = sizeY;

            if(c2y < 0) c2y = 0;
            if(c2y > sizeY) c2y = sizeY;

            generateDots();
        }       

        bool inImagette(std::pair<float,float> p){
            return(p.first >= startX && p.first < startX + sizeX && p.second >= startY && p.second < startY + sizeY );
        }

        void generateDots(){

            double sizeMinDot = 3.0;
            double lenMinDot = 5.0;

            dots = std::vector<std::pair<std::pair<int,int>,float>>();

            float len = std::max(lenMinDot,std::min(sizeX,sizeY) * 0.1);
            std::pair<float,float> c1 = std::make_pair(c1x+startX,c1y+startY);
            std::pair<float,float> c2 = std::make_pair(c2x+startX,c2y+startY);

            auto forward = normalize(c2-c1) * len;
            auto left = std::make_pair(-1*forward.second,forward.first);

            auto tmp = c1;

            int i = 1;
            while(inImagette(tmp)){
                while(inImagette(tmp)){
                    tmp = tmp + left;
                    dots.push_back({tmp,std::max(sizeMinDot,(float)len*0.3)});
                }
                do{
                    tmp = tmp - left;
                    dots.push_back({tmp,std::max(sizeMinDot,(float)len*0.3)});
                }while(inImagette(tmp));

                tmp = c1 + forward*i++;
            }            
        }

        Color getColor(int inx, int iny){

            int x = inx - startX;
            int y = iny - startY;
            
            

            float ux = x-c1x;
            float uy = y-c1y;

            float vx = c2x - c1x;
            float vy = c2y - c1y;

            float vd = vx*vx+vy*vy;

            float dot = ux*vx+uy*vy;

            float projX = dot/(vd) * vx;
            float projY = dot/(vd) * vy;

            float d = sqrt(pow(projX-c1x,2)+pow(projY-c1y,2));
            
            
            float distC1 = sqrt(pow(x-c1x,2)+pow(y-c1y,2));
            float distC2 = sqrt(pow(x-c2x,2)+pow(y-c2y,2));
            // if(distC1 < 5.0){
            //     return Color(0,0,0);
            // }
            // if(distC2 < 5.0){
            //     return Color(1,1,1);
            // }
            

            return Color::interpolate(startBackgroundColor, endBackgroundColor, d, sqrt(vd)-d);
        }

        Color getTramColor(int inx, int iny){
            for(auto dot : dots){
                if(len(dot.first - std::make_pair((float)inx,(float)iny)) < dot.second){
                    return dotColor;
                }
            }
            return avgColor;
        }

        Color getColor(std::pair<int,int> pos){
            return getColor(pos.first,pos.second);
        }

        Color getTramColor(std::pair<int,int> pos){
            return getTramColor(pos.first,pos.second);
        }

        static Image mergeFromImagettes(std::map<int,Imagette> imagettes, std::map<std::pair<int,int>,int> & zones, int width, int height){
            auto res = Image::Create(width,height,3);

            for(int y = 0 ; y < height ; y++ ){
                for(int x = 0 ; x < width ; x++){
                    auto pix = res.PixelAt(x,y);
                    auto zone = zones[std::make_pair(x,y)];

                    if(zone > 0){
                        auto img = imagettes[zone];
                        auto c = img.pixels[(y-img.startY)*img.width+(x-img.startX)];

                        if(c.exist){
                            pix[0] = c.r;
                            pix[1] = c.g;
                            pix[2] = c.b;
                            
                        }else{
                            std::cout<<"Le pixel n'existe pas x : "<<x<<" y : "<<y<<std::endl;

                            pix[0] = 1.0;
                            pix[1] = 1.0;
                            pix[2] = 1.0;
                        }
                    }else{
                        pix[0] = 0.0;
                        pix[1] = 0.0;
                        pix[2] = 0.0;
                    }
                }
            }

            return res;
        }

        static std::map<int,Imagette> splitZonesInImagettes(Image & l_image,std::map<std::pair<int,int>,int> & zones){
            int maxZone = zoneIndexMax(zones, l_image.width, l_image.height);
            std::cout<<"maxZone : "<<maxZone<<std::endl;

            auto usedZones = printUnUsedZones(zones,l_image.width, l_image.height);

            auto imagettes = std::map<int,Imagette>();

            for(int i = 0 ; i <= maxZone ; i ++){ // creer une imagette par zone

                if(!usedZones[i])
                continue;

                imagettes[i] = Imagette();

                imagettes[i].startX = l_image.width;
                imagettes[i].startY = l_image.height;

                imagettes[i].endX = 0;
                imagettes[i].endY = 0;
                

                imagettes[i].width  = 0;
                imagettes[i].height = 0;

                //imagettes[i].pixels = std::vector<Color>();
            }
            

            for(int y = 0 ; y < l_image.height ; y++){ // Trouver le min et le max de chaque zone
                for(int x = 0 ; x < l_image.width ; x++){
                    int z = zones[std::make_pair(x,y)];

                    imagettes[z].startX = std::min(x,imagettes[z].startX);
                    imagettes[z].startY = std::min(y,imagettes[z].startY);

                    imagettes[z].endX  = std::max(x,imagettes[z].endX);
                    imagettes[z].endY = std::max(y,imagettes[z].endY);
                }
            }

            for(int i = 0 ; i < maxZone ; i ++){ //calculer les dimensions de chaques zones
                if(!usedZones[i])
                continue;

                imagettes[i].width  = imagettes[i].endX - imagettes[i].startX+1;
                imagettes[i].height = imagettes[i].endY - imagettes[i].startY+1;

                if(imagettes[i].width <= 0 || imagettes[i].height <= 0){
                    std::cout<<"Une des dimensions est trop petite"<<std::endl
                    <<"w : "<<imagettes[i].width<<" h : "<<imagettes[i].height<<std::endl
                    <<"endX : "<<imagettes[i].endX<<" startX : "<<imagettes[i].startX<<std::endl
                    <<"endY : "<<imagettes[i].endY<<" startY : "<<imagettes[i].startY<<std::endl;

                    std::cout<<"Zone : "<<i<<std::endl;
                }
            }


            //remplir les imagettes avec les pixels

            for(int i = 0 ; i < maxZone ; i ++){

                if(!usedZones[i]){
                    continue;
                    std::cout<<"skipping"<<std::endl;
                }

                Imagette & img = imagettes[i];


                if(img.width < 0 || img.height < 0){
                    std::cout<<"Skip bug zone"<<std::endl;
                    continue;
                }

                img.pixels.resize(img.width*img.height);

                for(int y = img.startY ; y <= img.endY ; y ++){
                    for(int x = img.startX ; x <= img.endX ; x ++){
                        auto pos = std::make_pair(x,y);
                        Color c;

                        if(zones[pos] == i){
                            auto pix = l_image.PixelAt(pos);

                            if(l_image.channels == 3){
                                c = Color(pix[0],pix[1],pix[2]);
                            }else if(l_image.channels == 2){
                                c = Color(pix[0],pix[1],0);
                            }else if(l_image.channels == 1){
                                c = Color(pix[0],0,0);
                            }
                        }else{
                            c.exist = false;
                        }
                        img.pixels[(y-img.startY)*img.width+x-img.startX] = c;
                        //img.pixels.push_back(c);                        
                    }
                }
            }


            return imagettes;
        }

        static bool compareColorFloatPair(std::pair<float, Color> a, std::pair<float, Color> b){
            return(a.first < b.first);
        }

        static Texture extractTextureFromImagette(Imagette & img, Imagette & gradImg){
            Texture res;

            res.startX = img.startX;
            res.startY = img.startY;
            res.sizeX =  img.width;
            res.sizeY =  img.height;

            res.direction = 0;
            int nbpx = 0;
            for(int i = 0 ; i < gradImg.pixels.size() ; i ++){
                if(img.pixels[i].exist){
                    if(isnan(gradImg.pixels[i].g)){
                        std::cout<<"OULA : "<<i<<std::endl;
                    }else{
                        res.direction += gradImg.pixels[i].g;
                        nbpx++;
                    }
                }
            }


            auto tmpdir = res.direction;

            res.direction /= nbpx;


            if(isnan(res.direction)){
                res.direction = 0.0;
                std::cout<<"tmpdir : "<<tmpdir<<std::endl;
                std::cout<<"nb px  : "<<nbpx<<std::endl;
            }

            res.generate();

            int nbpxc1 = 0;
            int nbpxc2 = 0;

            Color tmp1;
            Color tmp2;
        
            auto sortedColors = std::vector<std::pair<float,Color>>();

            Color avgC = {0,0,0};

            for(int y = 0 ; y < img.height ; y ++){
                for(int x = 0 ; x < img.width ; x++){

                    if(img.pixels[y*img.width + x].exist){

                        float distC1 = sqrt(pow(x-res.c1x,2)+pow(y-res.c1y,2));
                        float distC2 = sqrt(pow(x-res.c2x,2)+pow(y-res.c2y,2));

                        sortedColors.push_back(std::make_pair(distC1-distC2, img.pixels[y*img.width + x] ));

                        avgC.r += img.pixels[y*img.width + x].r;
                        avgC.g += img.pixels[y*img.width + x].g;
                        avgC.b += img.pixels[y*img.width + x].b;
                    }
                }
            }

            avgC.r /= sortedColors.size();
            avgC.g /= sortedColors.size();
            avgC.b /= sortedColors.size();

            res.avgColor = avgC;

            res.dotColor.r = avgC.r * 0.5;
            res.dotColor.g = avgC.g * 0.5;
            res.dotColor.b = avgC.b * 0.5;

            std::sort(sortedColors.begin(),sortedColors.end(),compareColorFloatPair);

            float qttColorGrad = 0.1;
            int nbPix = std::max(qttColorGrad*sortedColors.size(),1.0f);

            for(int i = 0 ; i < nbPix ; i ++){
                tmp1.r += sortedColors[i].second.r;
                tmp1.g += sortedColors[i].second.g;
                tmp1.b += sortedColors[i].second.b;

                tmp2.r += sortedColors[sortedColors.size()-i-1].second.r;
                tmp2.g += sortedColors[sortedColors.size()-i-1].second.g;
                tmp2.b += sortedColors[sortedColors.size()-i-1].second.b;
            }



            if(sortedColors.size() == 0){

                res.startBackgroundColor = {1,1,1};
                res.endBackgroundColor = {1,1,1};

            }else{

                tmp1.r /= nbPix;
                tmp1.g /= nbPix;
                tmp1.b /= nbPix;

                tmp2.r /= nbPix;
                tmp2.g /= nbPix;
                tmp2.b /= nbPix;

                res.startBackgroundColor = tmp2;
                res.endBackgroundColor   = tmp1;
            }

            return res;
        }

        static std::map<int,Texture> extractTextures(Image l_image, std::map<std::pair<int,int>,int> zones){
            auto res = std::map<int,Texture>();

            auto grad = gradient(l_image);

            auto imagettes     = splitZonesInImagettes(l_image, zones);
            auto gradImagettes = splitZonesInImagettes(grad, zones);

            int maxZone = zoneIndexMax(zones, l_image.width, l_image.height);

            for(int i = 0 ; i < maxZone ; i ++){

                if(i%10==0)
                affProgressBar(i,0,maxZone,"Extraction des textures");

                res[i] = extractTextureFromImagette(imagettes[i], gradImagettes[i]);
                //res[i] = Texture();

                // res[i].startBackgroundColor = {1,0,0};
                // res[i].endBackgroundColor = {0,0,1};
            }

            return res;
        }

        static Image generateGradFromTextures(std::map<int, Texture> textures, std::map<std::pair<int,int>,int> zones, int width, int height){
            Image res = Image::Create(width, height, 3);

            for(int y = 0 ; y < height ; y ++){
                for(int x = 0 ; x < width ; x ++){
                    auto pos  = std::make_pair(x,y);
                    auto pix  = res.PixelAt(pos);
                    auto zone = zones[pos];
                    Color color;

                    if(zone==-1)
                        color = Color(0,1,0);
                    if(zone==0)
                        color = Color(1,1,0);
                    else
                        color = textures[zone].getColor(pos);

                    pix[0] = color.r;
                    pix[1] = color.g;
                    pix[2] = color.b;
                }
            }

            return res;
        }

        static Image generateTramFromTextures(std::map<int, Texture> textures, std::map<std::pair<int,int>,int> zones, int width, int height){
            Image res = Image::Create(width, height, 3);

            for(int y = 0 ; y < height ; y ++){
                for(int x = 0 ; x < width ; x ++){
                    auto pos  = std::make_pair(x,y);
                    auto pix  = res.PixelAt(pos);
                    auto zone = zones[pos];
                    Color color;

                    if(zone==-1)
                        color = Color(0,1,0);
                    if(zone==0)
                        color = Color(1,1,0);
                    else
                        color = textures[zone].getTramColor(pos);

                    pix[0] = color.r;
                    pix[1] = color.g;
                    pix[2] = color.b;
                }
            }

            return res;
        }
    };
}