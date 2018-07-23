#include "trace.hpp"
#include "../lib/model.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>


const std::tuple<int,int,int> adj[6] = {std::make_tuple(1,0,0),std::make_tuple(-1,0,0),std::make_tuple(0,1,0),std::make_tuple(0,-1,0),std::make_tuple(0,0,1),std::make_tuple(0,0,-1)};

int R;
std::vector<std::vector<std::vector<char>>> matrix;
std::vector<std::vector<std::vector<char>>> isGround;

std::vector<trace::command> solve(const std::vector<trace::command>& commands){
    std::vector<trace::command> opt;
    int x=0,y=0,z=0;
    bool harm = false;
    for(size_t i=0;i<commands.size();i++){
        const trace::command& cmd = commands[i];
        if(i==0 and cmd.type==trace::OP_FLIP) continue;

        if(cmd.type==trace::OP_HALT){
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_WAIT){
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_FLIP){
            if(harm) opt.emplace_back(cmd);  // TODO 最適化可能
        }else if(cmd.type==trace::OP_SMOV){
            int dx = cmd.data.lld.x;
            int dy = cmd.data.lld.y;
            int dz = cmd.data.lld.z;
            x += dx;
            y += dy;
            z += dz;
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_LMOV){
            int dx = cmd.data.sld.x1 + cmd.data.sld.x2;
            int dy = cmd.data.sld.y1 + cmd.data.sld.y2;
            int dz = cmd.data.sld.z1 + cmd.data.sld.z2;
            x += dx;
            y += dy;
            z += dz;
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_FUSS){
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_FILL){
            int posx = x + cmd.data.nd.x;
            int posy = y + cmd.data.nd.y;
            int posz = z + cmd.data.nd.z;

            if(!harm){  /* harmonicsがLowの場合は検査 */
                /* 
                 * 周囲に接地しているものがなければharmonicsをHighにする
                 */
                bool ground = false;
                for(int i=0;i<6;i++){
                    int dx,dy,dz;
                    std::tie(dx,dy,dz) = adj[i];
                    int nx = posx + dx;
                    int ny = posy + dy;
                    int nz = posz + dz;
                    if(nx<0 or ny<0 or nz<0 or nx>=R or ny>=R or nz>=R) continue;
                    if(isGround[nx][ny][nz]){
                        ground = true;
                        break;
                    }
                }

                if(!ground){    /* 接地していない */
                    harm = true;
                    opt.emplace_back(trace::opFlip());
                }else{          /* 接地している */
                    isGround[posx][posy][posz] = 1;
                }
            }
            matrix[posx][posy][posz] = 1;
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_VOID){
            int posx = x + cmd.data.nd.x;
            int posy = y + cmd.data.nd.y;
            int posz = z + cmd.data.nd.z;
            isGround[posx][posy][posz] = 0;
            matrix[posx][posy][posz] = 0;
            if(!harm){
                bool input=false;
                for(int i=0;i<6;i++){
                    if(harm) break;
                    int dx,dy,dz;
                    std::tie(dx,dy,dz) = adj[i];
                    // 消される場所の周囲
                    int cx = posx + dx;
                    int cy = posy + dy;
                    int cz = posz + dz;

                    // 範囲外,非オブジェクトは何もしない
                    if(cx<0 or cy<0 or cz<0 or cx>=R or cy>=R or cz>=R) continue;
                    if(!matrix[cx][cy][cz]) continue;

                    bool ground = false;
                    for(int j=0;j<6;j++){
                        int dx,dy,dz;
                        std::tie(dx,dy,dz) = adj[i];
                        int nx = cx + dx;
                        int ny = cy + dy;
                        int nz = cz + dz;
                        if(nx<0 or ny<0 or nz<0 or nx>=R or ny>=R or nz>=R) continue;
                        if(!matrix[nx][ny][nz]) continue;
                        if(isGround[nx][ny][nz]){
                            ground = true;
                            break;
                        }
                    }
                    if(!ground){
                        isGround[cx][cy][cz] = 0;
                        if(!input){
                            harm = true;
                            opt.emplace_back(trace::opFlip());
                        }
                    }
                }
            }
            opt.emplace_back(cmd);
        }else if(cmd.type==trace::OP_GFIL){
            std::cerr << "OP_GFIL is used" << std::endl;
            return std::vector<trace::command>();
        }else if(cmd.type==trace::OP_GVID){
            std::cerr << "OP_GVID is used" << std::endl;
            return std::vector<trace::command>();
        }
    }
    return opt;
}

inline void initGround(){
    isGround = matrix;
}

/*
 * argv[1] : 最適化するtrace
 * argv[2] : 最適化する対象のsrcモデル
 * argv[3] : 出力ファイル
 */
int main(int argc,char* argv[]){
    std::ifstream tracein(argv[1],std::ios::binary);
    auto commands = trace::decode(tracein);
    tracein.close();

    /* 最初にFLIPが無い場合は最適化を行わない */
    if(commands[0].type!=trace::OP_FLIP){
        return 0;
    }

    std::ifstream modelfile(argv[2],std::ios::binary);
    model::parse(matrix,R,modelfile);
    modelfile.close();

    initGround();

    auto optCmds = solve(commands);

    if(optCmds.empty()){
        return 0;
    }

    std::ofstream fout(argv[3],std::ios::binary);
    encode(optCmds,fout);
}
