namespace ImgCartoonizer {  

    struct Feature {
        int nbPixel;

        float r;
        float g;
        float b;

        float gradDir;
        float gradNorm;

        Feature(){
            nbPixel = 0;

            r = 0;
            g = 0;
            b = 0;

            gradDir = 0;
            gradNorm = 0;
        }
    };

    std::map<int,Feature> extractFeatures(Image l_image, std::map<std::pair<int,int>,int> zones){
        auto res = std::map<int,Feature>(); // les features de chaque zone
        int maxZone = zoneIndexMax(zones, l_image.width, l_image.height);

        for(int i = 0 ; i < maxZone ; i ++){
            res[i] = Feature();
        }

        for(int y = 0 ; y < l_image.height ; y ++){
            for(int x = 0 ; x < l_image.width ; x ++){
                auto p = l_image.PixelAt(x,y); // le pixel de l'image d'origine
                auto z = zones[std::make_pair(x,y)];

                res[z].nbPixel ++;

                res[z].r += *(p+0);
                res[z].g += *(p+1);
                res[z].b += *(p+2);
            }
        }

        for(int i = 0 ; i < maxZone ; i ++){
            res[i].r /= res[i].nbPixel;
            res[i].g /= res[i].nbPixel;
            res[i].b /= res[i].nbPixel;
        }       

        res[0].r = 0;
        res[0].g = 0;
        res[0].b = 0;

        return res;
    }

    Image generateFromFeatures(std::map<int,Feature> features, std::map<std::pair<int,int>,int> zones, int width, int height, int mode = 0, float contraste = 0.5){
        Image res = Image::Create(width,height,3);

        auto inImage = [](const std::pair<int,int>& p, const Image & l_image){
            return(p.first>=0 && p.second>=0 && p.first < l_image.width && p.second < l_image.height);
        };

        auto fourNei = std::vector<std::pair<int,int>>();
        fourNei.push_back({ 1, 0});
        fourNei.push_back({-1, 0});
        fourNei.push_back({ 0, 1});
        fourNei.push_back({ 0,-1});

        fourNei.push_back({ 1, 1});
        fourNei.push_back({-1,-1});
        fourNei.push_back({ 1,-1});
        fourNei.push_back({-1, 1});
        

        std::vector<std::pair<int,int>> borders =  std::vector<std::pair<int,int>>();

        for(int y = 0 ; y < height ; y ++){
            for(int x = 0 ; x < width ; x ++){

                auto p = res.PixelAt(x,y); //le pixel
                auto z = zones[std::make_pair(x,y)]; // la zone du pixel
                auto f = features[z];  // la feature de la zone du pixel

                if(z == 0){
                    borders.push_back(std::make_pair(x,y));
                }
                
                //couleur moyenne de la zone
                *(p+0) = f.r;
                *(p+1) = f.g;
                *(p+2) = f.b;

            }
        }

        for(auto b : borders){ // remplace la couleur des bordures par la moyenne des zones voisines
            int nnb = 0;

            float nr = 0;
            float ng = 0;
            float nb = 0;

            for(auto n : fourNei){
                auto nPos = b+n;

                if(inImage(nPos,res) && zones[nPos] != 0){
                    nnb++;
                    auto pix = res.PixelAt(nPos);

                    nr += *(pix+0);
                    ng += *(pix+1);
                    nb += *(pix+2);
                }
            }

            nr /= nnb;
            ng /= nnb;
            nb /= nnb;

            auto resPix = res.PixelAt(b);

            /*Mode:
                0 : noir
                1 : blanc
                2 : avg
                3 : inv avg
                4 : un peu moins que avg
                5 : un peu plus que avg
            */

            switch (mode)
            {
            case 0:
                *(resPix+0) = 0.0;
                *(resPix+1) = 0.0;
                *(resPix+2) = 0.0;
                break;
            case 1:
                *(resPix+0) = 1.0;
                *(resPix+1) = 1.0;
                *(resPix+2) = 1.0;
                break;
            case 2:
                *(resPix+0) = nr;
                *(resPix+1) = ng;
                *(resPix+2) = nb;
                break;
            case 3:
                *(resPix+0) = 1-nr;
                *(resPix+1) = 1-ng;
                *(resPix+2) = 1-nb;
                break;
            case 4:
                *(resPix+0) = nr * (1.0-contraste);
                *(resPix+1) = ng * (1.0-contraste);
                *(resPix+2) = nb * (1.0-contraste);
                break;
            case 5:
                *(resPix+0) = nr*(1.0-contraste)+contraste;
                *(resPix+1) = ng*(1.0-contraste)+contraste;
                *(resPix+2) = nb*(1.0-contraste)+contraste;
                break;
            }
        }

        return res;
    }

}