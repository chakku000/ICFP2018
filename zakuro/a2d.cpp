#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

#include "trace.hpp"
using namespace trace;

// not fully implemented


std::vector<command> flip(std::vector<command>& commands){
  std::vector<std::vector<command>> flipped;
  std::vector<command> tmp;
  int cnt = 1;
  int num = 1;
  std::queue<int> ms;
  for(int i = 0; i < commands.size(); i++){
    command& cmd = commands[i];
    switch(cmd.type) {
      case OP_HALT: {
        continue;
      }
      case OP_WAIT: {
        tmp.emplace_back(opWait());
        break;
      }
      case OP_FLIP: {
        tmp.emplace_back(opFlip());
        break;
      }
      case OP_SMOV: {
        command rev = opSMov(-cmd.data.lld.x, -cmd.data.lld.y, -cmd.data.lld.z);
        tmp.emplace_back(rev);
        break;
      }
      case OP_LMOV: {
        tmp.emplace_back(opLMov(-cmd.data.sld.x2, -cmd.data.sld.y2, -cmd.data.sld.z2, -cmd.data.sld.x1, -cmd.data.sld.y1, -cmd.data.sld.z1));
        break;
      }
      case OP_FUSP: {
        // TODO: 3体以上の場合
        num--;
        cnt--;
        if(commands[i+1].type == OP_FUSS) {
          int m = ms.front();
          ms.pop(); i++;
          tmp.emplace_back(opFiss(cmd.data.nd.x, cmd.data.nd.y, cmd.data.nd.z, m));
        } else {
          throw std::runtime_error("Assertion: FisP -> FusP");
        }
        break;
      }
      case OP_FISS: {
        num++;
        ms.push(cmd.data.ndm.m);
        tmp.emplace_back(opFusP(cmd.data.nd.x, cmd.data.nd.y, cmd.data.nd.z));
        tmp.emplace_back(opFusS(-cmd.data.nd.x, -cmd.data.nd.y, -cmd.data.nd.z));
        break;
      }
      case OP_FILL: {
        tmp.emplace_back(opVoid(cmd.data.nd.x, cmd.data.nd.y,  cmd.data.nd.z));
        break;
      }
      case OP_GFIL: {
        tmp.emplace_back(opGVid(cmd.data.nfd.nx, cmd.data.nfd.ny,  cmd.data.nfd.nz, cmd.data.nfd.fx, cmd.data.nfd.fy,  cmd.data.nfd.fz));
        break;
      }
      default:
        std::cout << cmd.type << std::endl;
        throw std::runtime_error("not implemented");
    }
    cnt--;
    if(cnt <= 0) {
      flipped.emplace_back(tmp);
      std::vector<command>().swap(tmp);
      cnt = num;
    }
  }
  std::reverse(flipped.begin(),flipped.end());
  std::vector<command> flatten;
  for(auto&& es: flipped){
    //std::reverse(es.begin(),es.end());
    for(auto && e: es){
      flatten.emplace_back(e);
    }
  }
  flatten.emplace_back(opHalt());
  return flatten;
}

// code for test
int main(int argc, char* argv[]) {
  std::ifstream fin(argv[1], std::ios::binary);
  std::ofstream fout(argv[2], std::ios::binary);
  //
  auto decoded = decode(fin);
  auto res = flip(decoded);
  /*
  print(decoded);
  std::cout << "-----" << std::endl;
  print(res);
  */
  encode(res, fout);
  fin.close();
  fout.close();
}