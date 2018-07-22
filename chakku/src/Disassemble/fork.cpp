#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "../lib/model.hpp"
#include "./trace.hpp"

struct Point{
    int x,y,z;
    Point(){}
    Point(int x,int y,int z) : x(x),y(y),z(z){}
};

int R;
std::vector<std::vector<std::vector<char>>> matrix;


Point pos[42];
std::set<int> seeds[42];


/*
 * argv[1] : model file
 */
int main(int argc,char* argv[]){
    if(argc<2){
        std::cerr << "Error 引数の数" << std::endl;
        return 1;
    }
    std::ifstream ifs(argv[1],std::ios::binary);
    model::parse(matrix,R,ifs);
    ifs.close();

    // Init
    pos[1]=Point(0,0,0);
    for(int i=2;i<=40;i++) seeds[1].insert(i);


    std::vector<trace::command> commands;
    // step1
    commands.emplace_back(trace::opFiss(1,0,0,4)); // 1 -> 2{3,4,5}
    commands.emplace_back(trace::opHalt());

    // step2
    commands.emplace_back(trace::opFiss(0,1,0,2)); // 1 -> 6{7}
    commands.emplace_back(trace::opFiss(0,1,0,2)); // 2 -> 3{4}

    // step3
    commands.emplace_back(trace::opFiss(0,0,1,1)); // 1 -> 8{}
    commands.emplace_back(trace::opFiss(0,0,1,1)); // 2 -> 5{}
    commands.emplace_back(trace::opFiss(0,0,1,1)); // 3 -> 4{}
    commands.emplace_back(trace::opFiss(0,0,1,1)); // 6 -> 7{}


    std::ofstream fout("FR001.nbt",std::ios::binary);
    encode(commands,fout);
}
