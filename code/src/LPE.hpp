#include <algorithm>
#include <map>
#include <queue>

void affProgressBar(int val, int min, int max, const char * label = ""){
    std::cout << "\x1B[2J\x1B[H";
    std::cout << label<<std::endl;
    std::cout <<(float)((int)((map(val,min,max,0,1000)))/(float)10)<<"%"<<std::endl;
    std::cout<<min<<" [";
    for(int j = 0 ; j < map(val,min,max,0,100);j++){
        std::cout<<"-";
    }
    std::cout<<">";
    for(int j = 0 ; j < 100-map(val,min,max,0,100);j++){
        std::cout<<" ";
    }
    std::cout<<"] "<<max<<std::endl;
}

void affProgressBar(int val, int min, int max, int val2, int min2, int max2, const char * label1 = "", const char * label2 = ""){
    std::cout << "\x1B[2J\x1B[H";

    std::cout << label1 <<std::endl;
    std::cout <<(float)((int)((map(val,min,max,0,1000)))/(float)10)<<"%"<<std::endl;
    std::cout<<min<<" [";
    for(int j = 0 ; j < map(val,min,max,0,100);j++){
        std::cout<<"-";
    }
    std::cout<<">";
    for(int j = 0 ; j < 100-map(val,min,max,0,100);j++){
        std::cout<<" ";
    }
    std::cout<<"] "<<max<<std::endl;

    std::cout << label2<<std::endl;
    std::cout <<(float)((int)((map(val2,min2,max2,0,1000)))/(float)10)<<"%"<<std::endl;
    std::cout<<min2<<" [";
    for(int j = 0 ; j < map(val2,min2,max2,0,100);j++){
        std::cout<<"-";
    }
    std::cout<<">";
    for(int j = 0 ; j < 100-map(val2,min2,max2,0,100);j++){
        std::cout<<" ";
    }
    std::cout<<"] "<<max2<<std::endl;
}



std::vector<float> HSVtoRGB(float hue, float sat,float val){
    int H = map(hue,0.,1,0.,360.);
    int S = map(sat,0.,1,0.,100.);
    int V = map(val,0.,1,0.,100.);

    if(H>360 || H<0 || S>100 || S<0 || V>100 || V<0){
        std::cout<<"The givem HSV values are not in valid range"<<std::endl;
    }
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }

    auto res = std::vector<float>{r+m,g+m,b+m};

    return res;
}

template <typename T,typename U>                                                   
std::pair<T,U> operator+(const std::pair<T,U> & l,const std::pair<T,U> & r) {   
    return {l.first+r.first,l.second+r.second};                                    
}


namespace ImgCartoonizer {  

    std::map<std::pair<int,int>,int> LPE(Image l_image, int steps = 50){

        auto fourNei = std::vector<std::pair<int,int>>();
        fourNei.push_back({ 1, 0});
        fourNei.push_back({-1, 0});
        fourNei.push_back({ 0, 1});
        fourNei.push_back({ 0,-1});

        fourNei.push_back({ 1, 1});
        fourNei.push_back({-1,-1});
        fourNei.push_back({ 1,-1});
        fourNei.push_back({-1, 1});
        

        auto inImage = [](const std::pair<int,int>& p, const Image & l_image){
            return(p.first>=0 && p.second>=0 && p.first < l_image.width && p.second < l_image.height);
        };

        auto res = ImgCartoonizer::Image::Create(l_image.width, l_image.height, 3);

        auto gradient = ImgCartoonizer::gradient(l_image);
        //auto gradient = ImgCartoonizer::contour(l_image); 

        //ImgCartoonizer::Image::Save("results/usedGradForSeg.png", gradient,0,1); 


        
        auto sortedImage = std::vector<std::pair<float,std::pair<int,int>>>();
        auto zones = std::map<std::pair<int,int>,int>();
        //-1 no bassin  
        // 0 LPE
        // n>0 bassin n

        float minVal = *gradient.PixelAt(0,0);
        float maxVal = minVal;

        for(int y = 0 ; y < l_image.height ; y ++){
            for(int x = 0 ; x < l_image.width ; x ++){
                float val = *gradient.PixelAt(x,y);
                sortedImage.push_back({val,{x,y}});
                zones[{x,y}] = -1;
                if(val>maxVal){
                    maxVal = val;
                }else if(val < minVal){
                    minVal = val;
                }
            }
        }

        // std::cout<<"min : "<<minVal<<std::endl;
        // std::cout<<"max : "<<maxVal<<std::endl;
        // return res;

        // minVal = 0;
        // maxVal = 1.0;


        std::sort(sortedImage.begin(),sortedImage.end());
        

        int nextPool = 1;

        
        auto traitable = std::queue<std::pair<float,std::pair<int,int>>>();
        auto in_queue_or_processed = std::map<std::pair<int,int>,bool>();

        for(auto p : sortedImage){
            in_queue_or_processed[p.second] = false;
        }


        int sortedInd = 0;
        int iteration = 0;
        float stepWidth = (maxVal) / steps;

        while(iteration<steps){
            auto etage  = std::vector<std::pair<float,std::pair<int,int>>>();

            float curentMaxVal = stepWidth*(iteration+1); // on détermine des bornes de l'étage
            
            while(sortedImage[sortedInd].first < curentMaxVal){ //on récupère les pixels de l'étage
                etage.push_back(sortedImage[sortedInd]);
                sortedInd++;
            }
            
            if(iteration != 0)
            for(int i = 0 ; i < etage.size() ; i ++){// on ajoute, dans la liste des pixels traitables, tout les pixels de l'étage qui ont des voisins labélisés
                auto p = etage[i];

                if(zones[p.second] != -1){
                    std::cout<<"mauvais étage"<<std::endl;
                    continue;
                }

                // if(i%10 == 0)
                // affProgressBar(i,0,etage.size(),"Pixels traitables");
                
                bool ttb = false;
                for(auto n : fourNei){
                    auto nCord = n+p.second;
                    if(inImage(nCord, l_image) && zones[nCord] > 0){
                        ttb = true;
                        break;
                    }
                }
                if(ttb){
                    traitable.push(p);
                    in_queue_or_processed[p.second] = true;
                }
            }


            int nbProcessed = 0;

            while(nbProcessed<etage.size()){ // tant qu'il reste des pixels non taités à l'étage on continue

                if(traitable.size()==0){ // s'il n'y a plus de pixels traitable, on creer un nouveau bassin
                    for(auto p : etage){
                        if(zones[p.second] == -1){
                            zones[p.second] = nextPool++;
                            in_queue_or_processed[p.second] = true;
                            nbProcessed++;

                            for(auto n : fourNei){ // et on rend ses voisins (du même étage) traitables
                                auto nCord = n+p.second;

                                if(!inImage(nCord, l_image)){
                                    continue;
                                }
                                
                                float val = *gradient.PixelAt(nCord);


                                if( val < curentMaxVal && zones[nCord] == -1 && !in_queue_or_processed[nCord]){
                                    traitable.push({val,nCord});
                                    in_queue_or_processed[nCord] = true;
                                }
                            }
                            break;
                        }
                    }  
                }

                while(traitable.size()>0){ // On traite tout les pixels traitables

                    if(nbProcessed%10==0)
                    affProgressBar(iteration,0,steps,nbProcessed,0,etage.size(),"Étage","Pixels de l'étage");

                    auto current = traitable.front();
                    traitable.pop();


                    if(zones[current.second] != -1){
                        std::cout<<"zones[current.second] != -1"<<std::endl;
                        //continue;
                    }
                    

                    //libélé le pixel //ajouter tout les voisins du même étage, non libélés dans la file
                    int state = -1;
                    for(auto n : fourNei){
                        auto nCord = n+current.second;

                        if(inImage(nCord, l_image)){
                            int nstate = zones[nCord];
                            float nval = *gradient.PixelAt(nCord);


                            if(state == 0 || nstate == -1 || nstate == 0 || nstate == state){
                                state = state;
                            }else if(state == -1 && nstate>0 ){
                                state = nstate;
                            }else if(state > 0 && nstate > 0){
                                if(state != nstate){
                                    state = 0;
                                }
                            }

                            if(!in_queue_or_processed[nCord] && nstate == -1 && nval < curentMaxVal){              
                                traitable.push({nval,nCord});
                                in_queue_or_processed[nCord] = true;
                            } 
                        }
                    }


                    if(zones[current.second] != -1){
                        std::cout<<"zones[current.second] != -1"<<std::endl;
                        //continue;
                    }else{
                        if(state == -1){
                            zones[current.second] = nextPool++;
                        }else{
                            zones[current.second] = state;
                        }
                    }

                    in_queue_or_processed[current.second] = true;
                    
                    nbProcessed++;
                }
                
            }
            iteration++;
        }

        //nextPool++;

        for(auto z : sortedImage){
            if(zones[z.second] == -1){
                std::cout<<"ALED"<<std::endl;
                zones[z.second] = nextPool;
            }
        }




        return zones;
    }

    int zoneIndexMax(std::map<std::pair<int,int>,int> zones, int width, int height){
        int max = 0;
        for(int y = 0 ; y < height ; y ++){
            for(int x = 0 ; x < width ; x++){
                if(zones[std::make_pair(x,y)] > max){
                    max = zones[std::make_pair(x,y)];
                }
            }
        }
        return max;
    }


    std::vector<bool> printUnUsedZones(std::map<std::pair<int,int>,int> zones, int width, int height){
        
        

        int max = zoneIndexMax(zones, width, height);
        auto usedZones = std::vector<bool>(max,false);

        for(int y = 0 ; y < height ; y++){
            for(int x = 0 ; x < width ; x++){
                auto z = zones[{x,y}];
                usedZones[z] = true;
            }
        }

        std::cout<<"Début printUnUsedZones"<<std::endl;

        for(int i = 0 ; i < max ; i ++){
            if(!usedZones[i]){
                std::cout<<"Zone inutilisée : "<<i<<std::endl;
            }
        }

        std::cout<<"Fin printUnUsedZones"<<std::endl;
        return usedZones;
    }

    Image fromZonesToImage(std::map<std::pair<int,int>,int> zones, int width, int height){

        Image res = Image::Create(width,height,3);

        int max = zoneIndexMax(zones, width, height);
        
        for(int y = 0 ; y < height ; y ++){
            for(int x = 0 ; x < width ; x ++){

                int i = y * width + x;
                auto pos = std::make_pair(x,y);

                if(zones[pos] == 0 || zones[pos] == -1){
                    res.data[3*i + 0] = 0;
                    res.data[3*i + 1] = 0;
                    res.data[3*i + 2] = 0;

                }else{
                    float val = map(zones[pos],0.0,(float)max,0.,1.);

                    auto c = HSVtoRGB(val,1,1);
                    

                    res.data[3*i + 0] = c[0];
                    res.data[3*i + 1] = c[1];
                    res.data[3*i + 2] = c[2];
                }
            }
        }
        

        return res;
    }
}