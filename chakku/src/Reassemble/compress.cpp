#include "trace.hpp"
#include <fstream>
#include <iostream>
#include <vector>

bool sameDirSmove(trace::command pre,trace::command cur){
    if(pre.data.lld.x and cur.data.lld.x) return true;
    if(pre.data.lld.y and cur.data.lld.y) return true;
    if(pre.data.lld.z and cur.data.lld.z) return true;
    return false;
}

int mlen(int x, int y, int z){
  return std::abs(x) + std::abs(y) + std::abs(z);
}

std::vector<trace::command> lmoveOPT(std::vector<trace::command>& commands){
  std::vector<trace::command> res;
  for (int i = 0; i < commands.size(); i++)
  {
    if(commands[i].type == trace::OP_SMOV) {
      auto lld1 = commands[i].data.lld;
      if(mlen(lld1.x, lld1.y, lld1.z) <= 5 && commands[i+1].type == trace::OP_SMOV) {
        auto lld2 = commands[i+1].data.lld;
        if(mlen(lld2.x, lld2.y, lld2.z) <= 5 &&
           ((lld1.x * lld2.x) + (lld1.y * lld2.y) + (lld1.z * lld2.z) == 0 )) {
          res.push_back(trace::opLMov(lld1.x, lld1.y, lld1.z, lld2.x, lld2.y, lld2.z));
          i++;
          continue;
        }
      }
    }
    res.push_back(commands[i]);
  }
  return res;
}


std::vector<trace::command> compress(const std::vector<trace::command> cmds){
    int x=0,y=0,z=0;
    std::vector<trace::command> ret;
    for(size_t i=0;i<cmds.size();i++){
        if(cmds[i].type == trace::OP_SMOV && 
                ((i-1>=0 && cmds[i-1].type != trace::OP_SMOV)
                 || (i-1>=0 && sameDirSmove(cmds[i-1],cmds[i])))){
            x += cmds[i].data.lld.x;
            y += cmds[i].data.lld.y;
            z += cmds[i].data.lld.z;
        }else{
            int tx=1,ty=1,tz=1;
            if(x<0) tx=-1;
            if(y<0) ty=-1;
            if(z<0) tz=-1;
            x=abs(x);
            y=abs(y);
            z=abs(z);
            while(x){
                int nx = std::min(15,x);
                ret.emplace_back(trace::opSMov(nx*tx,0,0));
                x -= nx;
            }
            while(y){
                int ny = std::min(15,y);
                ret.emplace_back(trace::opSMov(0,ny*ty,0));
                y -= ny;
            }
            while(z){
                int nz = std::min(15,z);
                ret.emplace_back(trace::opSMov(0,0,nz*tz));
                z -= nz;
            }

            if(cmds[i].type!=trace::OP_SMOV){
                ret.emplace_back(cmds[i]);
            }else{
                x += cmds[i].data.lld.x;
                y += cmds[i].data.lld.y;
                z += cmds[i].data.lld.z;
            }
        }
    }
    return ret;
}


int main(int argc,char* argv[]){
    //if(argc<3){
    //    std::cerr << "引数が足りない..." << std::endl;
    //    return 1;
    //}
    std::ifstream fin(argv[1],std::ios::binary);
    auto codes = trace::decode(fin);
    fin.close();

    //trace::print(codes);
    //std::cout << "-----------------------------" << std::endl;

    auto compressed = compress(codes);
    auto lmoveopt = lmoveOPT(compressed);
    //trace::print(compressed);
    std::ofstream fout(argv[2],std::ios::binary);
    encode(lmoveopt,fout);
}
