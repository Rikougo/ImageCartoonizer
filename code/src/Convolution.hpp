namespace ImgCartoonizer {
    struct Filter {
        float* data;
        int size;

        static Filter create(int size, float * data, float fact = 1.0){
            Filter res;
            res.size = size;
            res.data = data;

            if(fact != 1.0){
                for(int i = 0; i < size*size  ; i ++){
                    res.data[i] *= fact; 
                }
            }

            return res;
        }
    };

    Image filter(Image const & imgIn, Filter const & filter){
        Image res;
        res.data = std::vector<float>(imgIn.width * imgIn.height * imgIn.channels);

        res.width = imgIn.width;
        res.height = imgIn.height;
        res.channels = imgIn.channels;
        
        int deltaMin = -filter.size/2 ;
        int deltaMax =  filter.size/2 ;

        for(int y = 0 ; y < res.height ; y ++){
            for(int x = 0 ; x < res.width ; x ++){
                for(int c = 0 ; c < imgIn.channels ; c++){

                    float val = 0;

                    for(int dx = deltaMin ; dx <= deltaMax ; dx ++){
                        for(int dy = deltaMin ; dy <= deltaMax ; dy ++){
                            int nx = x+dx;
                            int ny = y+dy;

                            nx = (nx < 0 ? 0 : nx >= imgIn.width ? imgIn.width - 1 : nx );
                            ny = (ny < 0 ? 0 : ny >= imgIn.height ? imgIn.height - 1 : ny );

                            val += filter.data[(filter.size/2 + dy)*filter.size + filter.size/2 + dx] * imgIn.data[(ny*imgIn.width + nx)*imgIn.channels + c];

                        }
                    }
                    //res.data[(y*imgIn.width + x)*imgIn.channels + c] = std::min(std::max((int)(val+shift),0),255);
                    res.data[(y*imgIn.width + x)*imgIn.channels + c] = val;
                }
            }
        }
        
        return res;
    }

}