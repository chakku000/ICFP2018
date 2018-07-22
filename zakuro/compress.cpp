#include <iostream>
#include <fstream>
#include <vector>

#include "trace.hpp"
using namespace trace;

std::vector<command> solve(std::vector<command>& commands){
  int x = 0, y = 0, z = 0;
  std::vector<command> res;
  for (int i = 0; i < commands.size(); i++)
  {
    command& command = commands[i];
    if (command.type == OP_SMOV)
    {
      x += command.data.lld.x;
      y += command.data.lld.y;
      z += command.data.lld.z;
    } else if (command.type == OP_LMOV) {
      // TODO
    }
    else
    {
      int tx = 1, ty = 1, tz = 1;
      if (x < 0)
        tx = -1;
      if (y < 0)
        ty = -1;
      if (z < 0)
        tz = -1;
      x = abs(x);
      y = abs(y);
      z = abs(z);
      while (x)
      {
        int nx = std::min(15, x);
        res.push_back(opSMov(nx * tx, 0, 0));
        x -= nx;
      }
      while (y)
      {
        int ny = std::min(15, y);
        res.push_back(opSMov(0, ny * ty, 0));
        y -= ny;
      }
      while (z)
      {
        int nz = std::min(15, z);
        res.push_back(opSMov(0, 0, nz * tz));
        z -= nz;
      }

      res.push_back(commands[i]);
    }
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
  fin.close();
  fout.close();
}