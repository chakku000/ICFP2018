#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
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

int minx=300,miny=300,minz=300;
int maxx=0,maxy=0,maxz=0;

void moveX(int id,int toX,std::vector<trace::command>& commands){/*{{{*/
    int x = pos[id].x;
    int y = pos[id].y;
    int z = pos[id].z;
    if(x==toX) return;
    int dx = (toX-x)/abs(toX-x);
    while(x != toX){
        int nx=0;
        for(int i=1;i<=15;i++){
            if(matrix[x+i*dx][y][z]){
                nx = x + (i-1)*dx;
                break;
            }else if(x+i*dx==toX){
                nx = toX;
                break;
            }
            nx = x + i*dx;
        }

        // bot[id]は(x,y,nz)まで移動 ほかは待機
        if(nx>x){
            for(int i=1;i<=8;i++){
                if(i==id){ // nzまで進む
                    commands.emplace_back(trace::opSMov(nx-x,0,0));
                }else{ // 待機
                    commands.emplace_back(trace::opWait());
                }
            }
        }
        x = nx;

        // 障害ブロックを破壊
        if(x != toX and matrix[x+dx][y][z]){
            for(int i=1;i<=8;i++){
                if(i==id){
                    commands.emplace_back(trace::opVoid(dx,0,0));
                    matrix[x+dx][y][z]=0;
                }else{
                    commands.emplace_back(trace::opWait());
                }
            }
        }
    }
    pos[id].x = toX;
}/*}}}*/
void moveY(int id,int toY,std::vector<trace::command>& commands){/*{{{*/
    std::cerr << "moveY " << id << " " << toY << std::endl;
    int x = pos[id].x;
    int y = pos[id].y;
    int z = pos[id].z;
    if(y==toY) return;
    int dy = (toY-y)/abs(toY-y);
    while(y != toY){
        int ny=0;
        for(int i=1;i<=15;i++){
            if(matrix[x][y+i*dy][z]){
                ny = y + (i-1)*dy;
                break;
            }else if(y+i*dy==toY){
                ny = toY;
                break;
            }
            ny = y + i*dy;
        }
        if(ny>y){
            for(int i=1;i<=8;i++){
                if(i==id){ // nyまで進む
                    commands.emplace_back(trace::opSMov(0,ny-y,0));
                }else{ // 待機
                    commands.emplace_back(trace::opWait());
                }
            }
        }
        y = ny;
        if(y != toY and matrix[x][y+dy][z]){
            for(int i=1;i<=8;i++){
                if(i==id){
                    commands.emplace_back(trace::opVoid(0,dy,0));
                    matrix[x][y+dy][z]=0;
                }else{
                    commands.emplace_back(trace::opWait());
                }
            }
        }
    }
    pos[id].y = toY;
}/*}}}*/
void moveZ(int id,int toZ,std::vector<trace::command> &commands){/*{{{*/
    int x = pos[id].x;
    int y = pos[id].y;
    int z = pos[id].z;
    if(z==toZ) return;
    int dz = (toZ-z)/abs(toZ-z);
    while(z != toZ){
        int nz=0;
        for(int i=1;i<=15;i++){
            if(matrix[x][y][z+i*dz]){
                nz = z + (i-1)*dz;
                break;
            }else if(z+i*dz==toZ){
                nz = toZ;
                break;
            }
            nz = z + i*dz;
        }
        // nzはEmpty,nzの1つ先はFill(かも)

        // bot[id]は(x,y,nz)まで移動 ほかは待機
        if(nz>z){
            for(int i=1;i<=8;i++){
                if(i==id){ // nzまで進む
                    commands.emplace_back(trace::opSMov(0,0,nz-z));
                }else{ // 待機
                    commands.emplace_back(trace::opWait());
                }
            }
        }
        z = nz;

        // 障害ブロックを破壊
        if(z != toZ and matrix[x][y][z+dz]){
            for(int i=1;i<=8;i++){
                if(i==id){
                    commands.emplace_back(trace::opVoid(0,0,dz));
                    matrix[x][y][z+dz]=0;
                }else{
                    commands.emplace_back(trace::opWait());
                }
            }
        }
    }
    pos[id].z = toZ;
}/*}}}*/


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

    // Init{{{
    pos[1]=Point(0,0,0);

    std::vector<trace::command> commands;
    // step1
    commands.emplace_back(trace::opFiss(1,0,0,3)); // 1 -> 2{3,4,5,6}

    // step2
    commands.emplace_back(trace::opFiss(0,1,0,1)); // 1 -> 7{8,9}
    commands.emplace_back(trace::opFiss(0,1,0,1)); // 2 -> 3{4,5}

    // step3
    commands.emplace_back(trace::opFiss(0,0,1,0)); // 1 -> 8{}
    commands.emplace_back(trace::opFiss(0,0,1,0)); // 2 -> 5{}
    commands.emplace_back(trace::opFiss(0,0,1,0)); // 3 -> 4{}
    commands.emplace_back(trace::opFiss(0,0,1,0)); // 6 -> 7{}

    pos[1] = Point(0,0,0);
    pos[2] = Point(1,0,0);
    pos[3] = Point(1,1,0);
    pos[4] = Point(1,1,1);
    pos[5] = Point(1,0,1);
    pos[6] = Point(0,1,0);
    pos[7] = Point(0,1,1);
    pos[8] = Point(0,0,1);/*}}}*/

    // 最大値、最小値計算{{{
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            for(int k=0;k<R;k++){
                if(matrix[i][j][k]){
                    minx = std::min(i,minx);
                    miny = std::min(j,miny);
                    minz = std::min(k,minz);
                    maxx = std::max(i,maxx);
                    maxy = std::max(j,maxy);
                    maxz = std::max(k,maxz);
                }
            }
        }
    }/*}}}*/

    /*
     * 1:(0,0,0) 2:(1,0,0) 3:(1,1,0) 4:(1,1,1) 5:(1,0,1) 6:(0,1,0) 7:(0,1,1) 8:(0,0,1)
     */
    std::vector<Point> desire(9);
    int x1 = minx, x2 = std::min(maxx,std::min(minx+30,R));
    int y1 = maxy, y2 = std::max(maxy-30,0);
    int z1 = minz, z2 = std::min(maxz,std::min(minz+30,R));
    desire[1] = Point(x1,y2,z1);
    desire[2] = Point(x2,y2,z1);
    desire[3] = Point(x2,y1,z1);
    desire[4] = Point(x2,y1,z2);
    desire[5] = Point(x2,y2,z2);
    desire[6] = Point(x1,y1,z1);
    desire[7] = Point(x1,y1,z2);
    desire[8] = Point(x1,y2,z2);

    // 4番を配置
    // z軸方向
    moveZ(4,desire[4].z,commands);
    moveZ(5,desire[5].z,commands);
    moveZ(7,desire[7].z,commands);
    moveZ(8,desire[8].z,commands);

    moveZ(1,desire[1].z,commands);
    moveZ(2,desire[2].z,commands);
    moveZ(3,desire[3].z,commands);
    moveZ(6,desire[6].z,commands);

    // x軸方向
    moveX(2,desire[2].x,commands);
    moveX(3,desire[3].x,commands);
    moveX(4,desire[4].x,commands);
    moveX(5,desire[5].x,commands);

    moveX(1,desire[1].x,commands);
    moveX(6,desire[6].x,commands);
    moveX(7,desire[7].x,commands);
    moveX(8,desire[8].x,commands);

    // y軸方向
    moveY(3,desire[3].x,commands);
    moveY(4,desire[4].x,commands);
    moveY(6,desire[6].x,commands);
    moveY(7,desire[7].x,commands);

    moveY(1,desire[1].x,commands);
    moveY(2,desire[2].x,commands);
    moveY(5,desire[5].x,commands);
    moveY(8,desire[8].x,commands);

    std::ofstream fout("FD001.nbt",std::ios::binary);
    encode(commands,fout);
}
