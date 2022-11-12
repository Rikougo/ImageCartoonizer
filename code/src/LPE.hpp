#include <algorithm>
#include <map>
#include <queue>

void affProgressBar(int val, int min, int max){
    std::cout << "\x1B[2J\x1B[H";
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

void affProgressBar(int val, int min, int max, int val2, int min2, int max2){
    std::cout << "\x1B[2J\x1B[H";

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

    Image LPE(Image l_image, int steps = 10){

        auto fourNei = std::vector<std::pair<int,int>>();
        fourNei.push_back({ 1, 0});
        fourNei.push_back({-1, 0});
        fourNei.push_back({ 0, 1});
        fourNei.push_back({ 0,-1});

        // fourNei.push_back({ 1, 1});
        // fourNei.push_back({-1,-1});
        // fourNei.push_back({ 1,-1});
        // fourNei.push_back({-1, 1});

        auto inImage = [](std::pair<int,int> p, Image l_image){
            return(p.first>=0 && p.second>=0 && p.first < l_image.width && p.second < l_image.height);
        };

        auto res = ImgCartoonizer::Image::Create(l_image.width, l_image.height, 3);

        auto gradient = ImgCartoonizer::gradient(l_image);

        
        auto sortedImage = std::vector<std::pair<float,std::pair<int,int>>>();

        for(int y = 0 ; y < l_image.height ; y ++){
            for(int x = 0 ; x < l_image.width ; x ++){
                sortedImage.push_back({gradient.data[2*(y*gradient.width + x)+0],{x,y}});
            }
        }

        std::sort(sortedImage.begin(),sortedImage.end());

        auto zones = std::vector<int>(l_image.width*l_image.height,-1);
        //-1 no bassin
        // 0 LPE
        // n>0 bassin n
        
        float stepWidth = 1.0 / steps;
        

        int nextPool = 0;

        
        auto traitable = std::queue<std::pair<float,std::pair<int,int>>>();
        auto nextTraitable = std::queue<std::pair<float,std::pair<int,int>>>();
        auto in_queue_or_processed = std::map<std::pair<int,int>,bool>();

        for(auto p : sortedImage){
            in_queue_or_processed[p.second] = false;
        }

        int i = 0;
        int iteration = 0;

        while(iteration<steps){
            auto etage  = std::vector<std::pair<float,std::pair<int,int>>>();

            float curentMaxVal = stepWidth*iteration;
            float nextMaxVal   = stepWidth*(iteration+1);
            
            while(sortedImage[i].first < curentMaxVal){
                etage.push_back(sortedImage[i]);
                i++;
            }


            int nbProcessed = 0;

            while(nbProcessed<etage.size()){
                while(traitable.size()>0){
                    if(nbProcessed%100==0)
                    affProgressBar(iteration,0,steps,nbProcessed,0,etage.size());
                    //std::cout<<"traitable.size() : "<<traitable.size()<<std::endl;

                    //std::cout<<"+1"<<std::endl;

                    auto current = traitable.front();
                    traitable.pop();
                    nbProcessed++;

                    //libélé le pixel //ajouter tout les voisins du même étage, non libélés dans la file
                    int state = -1;
                    for(auto n : fourNei){
                        auto nCord = n+current.second;

                        if(inImage(nCord, l_image)){
                            int nstate = zones[nCord.second*l_image.width + nCord.first];
                            float nval = *gradient.PixelAt(nCord);
                            state = nstate == -1 || nstate == 0 || nstate == state  ? state : state == -1 ? nstate : 0 ;

                            if(!in_queue_or_processed[nCord] && nstate == -1){
                                if(nval < curentMaxVal){
                                    traitable.push({nval,nCord});
                                    in_queue_or_processed[nCord] = true;
                                }else if(nval < nextMaxVal){
                                    nextTraitable.push({nval,nCord});
                                    in_queue_or_processed[nCord] = true;
                                }
                            } 
                        }

                        if(state==0)
                        break;
                    }
                    zones[current.second.second*l_image.width + current.second.first] = (state == -1 ? nextPool++ : state);
                }
                for(auto p : etage){
                    if(!in_queue_or_processed[p.second]){
                        traitable.push(p);
                        in_queue_or_processed[p.second] = true;
                        break;
                    }
                } 
            }

            traitable = nextTraitable;
            nextTraitable = std::queue<std::pair<float,std::pair<int,int>>>();

            iteration++;
        }

        {
            //vérifier si tout les pixel de l'étage sont traités
            //remettre les pixels non traités dans le vecteur image

            /*
            
            for(int ind = 0 ; ind < sortedImage.size() ; ind++){
                if(ind % 100 == 0)
                affProgressBar(ind,0,sortedImage.size());
                auto value= sortedImage[ind];

                int x = value.second.first;
                int y = value.second.second;

                int state = -1;
                
                for(auto n : fourNei){

                    int nx = x+n.first;
                    int ny = y+n.second;

                    if(nx < l_image.width && nx >= 0 && ny < l_image.height && ny >= 0){
                        int vstate = zones[ny*l_image.width + nx];

                        //auto sameFloor = std::abs(value.first - *l_image.PixelAt(nx,ny)) < stepWidth;

                        state = vstate == -1 || vstate == 0 || vstate == state  ? state : state == -1 ? vstate : 0 ;

                        if(sameFloor && vstate>0 && state>0 && state!=vstate){
                            for(int i = 0 ; i < zones.size() ; i ++){
                                if(zones[i] == state){
                                    zones[i] = vstate;
                                }
                            }
                        }

                        if(state==0)
                        break;  
                    }
                }

                if(state == -1){
                    zones[y*l_image.width + x] = nextPool++;
                }else{
                    zones[y*l_image.width + x] =  state;     
                }   
            }
            */

            /*
            auto newZone = std::vector<int>();

            for(int i = 0 ; i < nextPool ; i ++){
                newZone.push_back(i);
            }

            
            for(int i = 0 ; i< fusion.size() ; i ++){
                auto f = fusion[i];
                int toReplaceval = newZone[f.second]; 
                int newval = newZone[f.first];
                affProgressBar(i,0,fusion.size());
                for(int i = 0 ; i < newZone.size() ; i++){
                    if(newZone[i] == toReplaceval){
                        newZone[i] = newval;
                    }
                }
            }
            */

        }

        for(int i = 0 ; i < zones.size() ; i ++){

            if(false && zones[i] == -1){
                std::cout<<"OSKOUR"<<std::endl;
                std::cout<<"i : "<<i<<std::endl;
            }

            //zones[i] = newZone[zones[i]];

            if(zones[i] == 0 || zones[i] == -1){
                res.data[3*i + 0] = 0;
                res.data[3*i + 1] = 0;
                res.data[3*i + 2] = 0; 
            }else{
                float val = map(zones[i],0.0,(float)nextPool,0.,1.);

                auto c = HSVtoRGB(val,1,1);
                

                res.data[3*i + 0] = c[0];
                res.data[3*i + 1] = c[1];
                res.data[3*i + 2] = c[2];
            }

        }
        

        return res;
    }
}