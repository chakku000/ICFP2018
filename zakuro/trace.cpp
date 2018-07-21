#include <iostream>
#include <fstream>
#include <vector>

#define OP_HALT 1
#define OP_WAIT 2
#define OP_FLIP 3
#define OP_SMOV 4
#define OP_LMOV 5
#define OP_FUSP 6
#define OP_FUSS 7
#define OP_FISS 8
#define OP_FILL 9

union command_data {
  struct { int x, y, z; } lld;
  struct { int x1, y1, z1; int x2, y2, z2; } sld;
  struct { int x, y, z; } nd;
  struct { int x, y, z; int m; } ndm;
};
struct command {
  int type;
  command_data data;
  command(int type) : type(type) {}
  command(int type, command_data data) : type(type), data(data) {}
};

command_data decode_lld(unsigned char a, unsigned char i){
  switch(a) {
    case 0b01:
      return { .lld = {(int)i-15, 0, 0} };
    case 0b10:
      return { .lld = {0, (int)i-15, 0} };
    case 0b11:
      return { .lld = {0, 0, (int)i-15} };
  }
}

command_data decode_sld(unsigned char a1, unsigned char i1, unsigned char a2, unsigned char i2){
  command_data d = { .sld = { 0, 0, 0, 0, 0, 0 } };
  if(a1 == 0b01) d.sld.x1 = (int)i1-5;
  if(a1 == 0b10) d.sld.y1 = (int)i1-5;
  if(a1 == 0b11) d.sld.z1 = (int)i1-5;
  if(a2 == 0b01) d.sld.x2 = (int)i2-5;
  if(a2 == 0b10) d.sld.y2 = (int)i2-5;
  if(a2 == 0b11) d.sld.z2 = (int)i2-5;
}


command_data decode_nd(unsigned char nd, int m){
  if(m == -1) {
    return { .nd = { (int)nd/9 - 1, (int)(nd % 9) / 3 - 1, (int)(nd % 3) - 1 } };
  } else {
    return { .ndm = { (int)nd/9 - 1, (int)(nd % 9) / 3 - 1, (int)(nd % 3) - 1 , m } };
  }
}

std::vector<command> decode(std::ifstream& fin){
  std::vector<command> cmds;
  while(!fin.eof()) {
    unsigned char c = fin.get();
    if(c == 0b11111111) {
      //std::cout << "HALT" << std::endl;
      cmds.push_back(command(OP_HALT));
      break;
    } else if (c == 0b11111110) {
      //std::cout << "WAIT" << std::endl;
      cmds.push_back(command(OP_WAIT));
    } else if (c == 0b11111101) {
      //std::cout << "FLIP" << std::endl;
      cmds.push_back(command(OP_FLIP));
    } else if((c & 0b1111) == 0b0100){
      unsigned char llda = (c >> 4) & 0b11;
      c = fin.get();
      unsigned char lldi = c & 0b11111;
      command_data lld = decode_lld(llda, lldi);
      //std::cout << "SMove" << ' '  << lld.lld.x << std::endl;
      cmds.push_back(command(OP_SMOV, lld));
    } else if((c & 0b1111) == 0b1100){
      unsigned char sld2a = (c >> 6) & 0b11;
      unsigned char sld1a = (c >> 4) & 0b11;
      c = fin.get();
      unsigned char sld2i = (c >> 4) & 0b1111;
      unsigned char sld1i = c & 0b1111;
      command_data sld = decode_sld(sld1a, sld1i, sld1a, sld2i);
      //std::cout << "LMove" << std::endl;
      cmds.push_back(command(OP_LMOV, sld));
    } else if((c & 0b111) == 0b111){
      unsigned char nd = (c >> 3) & 0b11111;
      command_data ndd = decode_nd(nd, -1);
      //std::cout << "FusP" << std::endl;
      cmds.push_back(command(OP_FUSP, ndd));
    } else if((c & 0b111) == 0b110){
      unsigned char nd = (c >> 3) & 0b11111;
      command_data ndd = decode_nd(nd, -1);
      //std::cout << "FusS" << std::endl;
      cmds.push_back(command(OP_FUSS, ndd));
    } else if((c & 0b111) == 0b101){
      unsigned char nd = (c >> 3) & 0b11111;
      unsigned char m = fin.get();
      command_data ndd = decode_nd(nd, m);
      //std::cout << "Fis" << std::endl;
      cmds.push_back(command(OP_FISS, ndd));
    } else if((c & 0b111) == 0b011){
      unsigned char nd = (c >> 3) & 0b11111;
      command_data ndd = decode_nd(nd, -1);
      //std::cout << "Fil" << ndd.nd.x << ndd.nd.y << ndd.nd.z << std::endl;
      cmds.push_back(command(OP_FILL, ndd));
    }
  }
  return cmds;
}

void encode(std::vector<command> cmds, std::ofstream& fout){
  for(command& cmd: cmds){
    int t = cmd.type;
    command_data& d = cmd.data;
    if(t == OP_HALT) {
      fout.put(0b11111111);
    } else if(t == OP_WAIT) {
      fout.put(0b11111110);
    } else if(t == OP_FLIP) {
      fout.put(0b11111101);
    } else if(t == OP_SMOV) {
      unsigned char a, i;
           if(d.lld.x != 0) a = 0b01, i = d.lld.x + 15;
      else if(d.lld.y != 0) a = 0b10, i = d.lld.y + 15;
      else if(d.lld.z != 0) a = 0b11, i = d.lld.z + 15;
      fout.put((a << 4) | 0b0100);
      fout.put(i & 0b11111);
    } else if(t == OP_LMOV) {
      unsigned char a1, i1, a2, i2;
           if(d.sld.x1 != 0) a1 = 0b01, i1 = d.sld.x1 + 5;
      else if(d.sld.y1 != 0) a1 = 0b10, i1 = d.sld.y1 + 5;
      else if(d.sld.z1 != 0) a1 = 0b11, i1 = d.sld.z1 + 5;
           if(d.sld.x2 != 0) a2 = 0b01, i2 = d.sld.x2 + 5;
      else if(d.sld.y2 != 0) a2 = 0b10, i2 = d.sld.y2 + 5;
      else if(d.sld.z2 != 0) a2 = 0b11, i2 = d.sld.z2 + 5;
      fout.put((a2 << 6) | (a2 << 4) | 0b1100);
      fout.put((i2 << 4) | i1);
    } else if(t == OP_FUSP){
      unsigned char nd = (d.nd.x + 1) * 9 + (d.nd.y + 1) * 3 + (d.nd.z + 1);
      fout.put((nd << 3) | 0b111);
    } else if(t == OP_FUSS){
      unsigned char nd = (d.nd.x + 1) * 9 + (d.nd.y + 1) * 3 + (d.nd.z + 1);
      fout.put((nd << 3) | 0b110);
    } else if(t == OP_FISS){
      unsigned char nd = (d.ndm.x + 1) * 9 + (d.ndm.y + 1) * 3 + (d.ndm.z + 1);
      fout.put((nd << 3) | 0b101);
      fout.put(d.ndm.m);
    } else if(t == OP_FILL){
      unsigned char nd = (d.nd.x + 1) * 9 + (d.nd.y + 1) * 3 + (d.nd.z + 1);
      fout.put((nd << 3) | 0b011);
    }
  }
}

// code for test
int main(int argc, char* argv[]) {
  std::ifstream fin(argv[1], std::ios::binary);
  std::ofstream fout(argv[2], std::ios::binary);
  encode(decode(fin), fout);
  fin.close();
}