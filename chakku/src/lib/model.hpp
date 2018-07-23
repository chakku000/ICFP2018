#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

namespace model{
    void parse(std::vector<std::vector<std::vector<char>>>& matrix,int& R,std::ifstream& ifs){
        char tmp;
        ifs.read(&tmp,sizeof(tmp));
        R=tmp;
        matrix = std::vector<std::vector<std::vector<char>>>(R,std::vector<std::vector<char>>(R,std::vector<char>(R,0)));

        int idx=0;
        while(!ifs.eof()){
            ifs.read(&tmp,sizeof(tmp));
            for(int i=0;i<8;i++){
                if((tmp>>i)&1){
                    int x=idx/(R*R);
                    int y=(idx%(R*R))/R;
                    int z=idx%R;
                    matrix[x][y][z] = 1;
                }
                idx++;
            }
        }
    }
};

#endif
