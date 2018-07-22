#include <iostream>
#include <fstream>
#include <vector>

#include "trace.hpp"
using namespace trace;

int mlen(int x, int y, int z){
  return std::abs(x) + std::abs(y) + std::abs(z);
}

std::vector<command> solve(std::vector<command>& commands){
  std::vector<command> res;
  for (int i = 0; i < commands.size(); i++)
  {
    if(commands[i].type == OP_SMOV) {
      auto lld1 = commands[i].data.lld;
      if(mlen(lld1.x, lld1.y, lld1.z) <= 5 && commands[i+1].type == OP_SMOV) {
        auto lld2 = commands[i+1].data.lld;
        if(mlen(lld2.x, lld2.y, lld2.z) <= 5 &&
           ((lld1.x * lld2.x) + (lld1.y * lld2.y) + (lld1.z * lld2.z) == 0 )) {
          res.push_back(opLMov(lld1.x, lld1.y, lld1.z, lld2.x, lld2.y, lld2.z));
          i++;
          continue;
        }
      }
    }
    res.push_back(commands[i]);
  }
  return res;
}

// code for test
int main(int argc, char* argv[]) {
  std::ifstream fin(argv[1], std::ios::binary);
  std::ofstream fout(argv[2], std::ios::binary);
  std::vector<command> decoded = decode(fin);
  std::vector<command> res = solve(decoded);
  print(res);
  encode(res, fout);
  fin.close();
  fout.close();
}