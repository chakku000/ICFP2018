#include "trace.hpp"
#include <fstream>
#include <iostream>
#include <vector>


int main(int argc,char** argv){
    std::ifstream fin(argv[1],std::ios::binary);
    auto codes = trace::decode(fin);
    trace::print(codes);
}
