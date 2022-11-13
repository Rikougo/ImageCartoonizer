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
        auto zones = std::map<std::pair<int,int>,int>();
        //-1 no bassin
        // 0 LPE
        // n>0 bassin n

        float minVal = gradient.data[2*(0*gradient.width + 0)+0];
        float maxVal = minVal;

        for(int y = 0 ; y < l_image.height ; y ++){
            for(int x = 0 ; x < l_image.width ; x ++){
                auto val = gradient.data[2*(y*gradient.width + x)+0];
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


        std::sort(sortedImage.begin(),sortedImage.end());
        

        int nextPool = 0;

        
        auto traitable = std::queue<std::pair<float,std::pair<int,int>>>();
        auto in_queue_or_processed = std::map<std::pair<int,int>,bool>();

        for(auto p : sortedImage){
            in_queue_or_processed[p.second] = false;
        }


        int i = 0;
        int iteration = 0;
        float stepWidth = (maxVal) / steps;

        while(iteration<steps){
            auto etage  = std::vector<std::pair<float,std::pair<int,int>>>();

            float curentMaxVal = stepWidth*(iteration+1); // on détermine des bornes de l'étage
            
            while(sortedImage[i].first < curentMaxVal){ //on récupère les pixels de l'étage
                etage.push_back(sortedImage[i]);
                i++;
            }
            
            if(iteration != 0)
            for(int i = 0 ; i < etage.size() ; i ++){// on ajoute, dans la liste des pixels traitables, tout les pixels de l'étage qui ont des voisins labélisés
                auto p = etage[i];

                if(i%10 == 0)
                affProgressBar(i,0,etage.size(),"Pixels traitables");
                
                bool ttb = false;
                for(auto n : fourNei){
                    auto nCord = n+p.second;
                    if(inImage(nCord, l_image) && zones[p.second] > 0){
                        ttb = true;
                        break;
                    }
                }
                if(ttb){
                    traitable.push(p);
                }
            }


            int nbProcessed = 0;

            while(nbProcessed<etage.size()){ // tant qu'il reste des pixels non taités à l'étage on continue

                if(traitable.size()==0){ // si il n'y a plus de pixels traitable, on creer un nouveau bassin
                    for(auto p : etage){
                        if(zones[p.second] == -1){
                            zones[p.second] = nextPool++;
                            nbProcessed++;

                            for(auto n : fourNei){ // et on rend ses voisins (du même étage) traitables
                                auto nCord = n+p.second;
                                float val = *gradient.PixelAt(nCord);
                                if(inImage(nCord, l_image) && val < curentMaxVal && zones[nCord] == -1){
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
                    

                    //libélé le pixel //ajouter tout les voisins du même étage, non libélés dans la file
                    int state = -1;
                    for(auto n : fourNei){
                        auto nCord = n+current.second;

                        if(inImage(nCord, l_image)){
                            int nstate = zones[nCord];
                            float nval = *gradient.PixelAt(nCord);
                            state = (nstate == -1 || nstate == 0 || nstate == state ) ? state : state == -1 ? nstate : 0 ;

                            if(!in_queue_or_processed[nCord] && nstate == -1){
                                if(nval < curentMaxVal){
                                    traitable.push({nval,nCord});
                                    in_queue_or_processed[nCord] = true;
                                }
                            } 
                        }

                        if(state==0)
                        break;
                    }
                    zones[current.second] = (state == -1 ? nextPool++ : state);
                    nbProcessed++;
                    
                    if(zones[current.second] == -1){
                        std::cout<<"what is the actual fuck ???"<<std::endl;
                        return res;
                    }
                }
                
            }
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



        for(int y = 0 ; y < l_image.height ; y ++){
            for(int x = 0 ; x < l_image.width ; x ++){

                int i = y * l_image.width + x;
                auto pos = std::make_pair(x,y);

                if(zones[pos] == -1){
                    std::cout<<"OSKOUR"<<std::endl;
                    std::cout<<"i : "<<i<<std::endl;
                    std::cout<<"val :"<<gradient.data[i]<<std::endl;
                }

                //zones[i] = newZone[zones[i]];

                if(zones[pos] == 0 || zones[pos] == -1){
                    res.data[3*i + 0] = 0;
                    res.data[3*i + 1] = 0;
                    res.data[3*i + 2] = 0; 
                }else{
                    float val = map(zones[pos],0.0,(float)nextPool,0.,1.);

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