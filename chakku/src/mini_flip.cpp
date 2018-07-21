#include <bits/stdc++.h>
using namespace std;
/*{{{*/  //template
#define rep(i,n) for(int i=0;i<(int)(n);i++)
constexpr int INF = numeric_limits<int>::max()/2;
constexpr long long LINF = numeric_limits<long long>::max()/3;
#define mp make_pair
#define pb push_back
#define eb emplace_back
#define fi first
#define se second
#define all(v) (v).begin(),(v).end()
#define sz(x) (int)(x).size()
#define debug(x) cerr<<#x<<":"<<x<<endl
#define debug2(x,y) cerr<<#x<<","<<#y":"<<x<<","<<y<<endl
//struct fin{ fin(){ cin.tie(0); ios::sync_with_stdio(false); } } fin_;
struct Double{ double d; explicit Double(double x) : d(x){} };
ostream& operator<<(ostream& os,const Double x){ os << fixed << setprecision(20) << x.d; return os; }
template<typename T> ostream& operator<<(ostream& os,const vector<T>& vec){ os << "["; for(const auto& v : vec){ os << v << ","; } os << "]"; return os; }
template<typename T,typename U> ostream& operator<<(ostream& os,const pair<T,U>& p){ os << "(" << p.first << ","<< p.second <<")"; return os; }
template<typename T> ostream& operator<<(ostream& os,const set<T>& st){ os<<"{"; for(T v:st) os<<v<<","; os <<"}"; return os; }
template<typename T,typename U> inline void chmax(T &x,U y){ if(y>x) x = y; }
template<typename T,typename U> inline void chmin(T &x,U y){ if(y<x) x = y; }
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector<vi> vvi;
ll gcd(ll a,ll b){ if(b==0) return a; else return gcd(b,a%b); }
//constexpr double eps = 1e-14; 
constexpr double eps = 1e-10; 
constexpr ll mod = 1e9+7;
/*}}}*/

tuple<int,int,int> adj[] = {make_tuple(1,0,0),make_tuple(-1,0,0),make_tuple(0,1,0),make_tuple(0,-1,0),make_tuple(0,0,1),make_tuple(0,0,-1)};

string Fmt(const string& s,int x,int y,int z){ return  s + " " + to_string(x) + " " + to_string(y) + " " + to_string(z); }

using P = tuple<int,int,int>;

/* 文字列Split *//*{{{*/
vector<string> split(string str,char delim){
    for(char& c : str) if(c==delim) c=' ';
    stringstream ss;
    ss << str;
    string s;
    vector<string> res;
    while(ss >> s){
        res.push_back(s);
    }
    return res;
}/*}}}*/

bool isground[251][251][251];
bool harmonic = false;

void solve(int R,vector<string>& commands){
    int x=0,y=0,z=0;
    int nowx=0,nowy=0,nowz=0;
    vector<string> ans;
    //cout << commands.size() << endl;
    for(int i=0;i<commands.size();i++){
        //cerr << i << endl;
        auto command = split(commands[i],' ');
        if(command[0] == "Flip") continue;
        if(command[0]=="SMove" or command[0]=="LMove"){
            x += stoi(command[1]);
            y += stoi(command[2]);
            z += stoi(command[3]);
        }else{
            int tx=1,ty=1,tz=1;
            if(x<0) tx=-1;
            if(y<0) ty=-1;
            if(z<0) tz=-1;
            nowx += x;
            nowy += y;
            nowz += z;
            x=abs(x);
            y=abs(y);
            z=abs(z);
            while(x){
                int nx = min(15,x);
                cout<< Fmt("SMove",nx*tx,0,0) << endl;
                x -= nx;
            }
            while(y){
                int ny = min(15,y);
                cout << Fmt("SMove",0,ny*ty,0) << endl;
                y -= ny;
            }
            while(z){
                int nz = min(15,z);
                cout << Fmt("SMove",0,0,nz*tz) << endl;
                z -= nz;
            }

            /* 埋める前に,埋めたときにその場所がgrounded出ないかどうかを判定しgrounded出ない場合はFlipしてから埋める */
            if(command[0] == "Fill"){
                int filx = nowx + stoi(command[1]);
                int fily = nowy + stoi(command[2]);
                int filz = nowz + stoi(command[3]);
                if(fily == 0){
                    isground[filx][fily][filz] = true;
                }
                for(int j=0;j<6;j++){
                    int dx,dy,dz;
                    tie(dx,dy,dz) = adj[j];
                    int bx = filx + dx;
                    int by = fily + dy;
                    int bz = filz + dz;
                    if(bx>=0 and by>=0 and bz>=0 and bx<R and by<R and bz<R and isground[bx][by][bz]){
                        isground[filx][fily][filz] = true;
                        break;
                    }
                }
                if(!isground[filx][fily][filz] and !harmonic){
                    harmonic = true;
                    cout << "Flip" << endl;
                }
            }

            if(command[0] == "Halt" and harmonic){
                harmonic = false;
                cout << "Flip" << endl;
            }
            cout << commands[i] << endl;
            //ans.push_back(commands[i]);
        }
    }
}

int main(int argc,char** argv){
    if(argc<=2){
        cerr << "引数氏〜〜" << endl;
        return 1;
    }
    vector<P> boxes;

    std::string str;
    vector<string> commands;
    char R;
    {
        std::ifstream model(argv[2],std::ios::binary);
        if(!model){
            std::cerr << "Model File Open Error" << std::endl;
            return -1;
        }
        model.read(&R,sizeof(R));
        //cout << (int)R << endl;
        char tmp;

        // xR^2 + yR + z
        int idx=0;
        while(!model.eof()){
            model.read(&tmp,sizeof(tmp));
            for(int i=0;i<8;i++){
                if((tmp>>i)&1){
                    int x = idx / (R*R);
                    int y = (idx%(R*R)) / R;
                    int z = idx % R;
                    boxes.push_back(P(x,y,z));
                }
                idx++;
            }
        }
    }

    std::ifstream ifs(argv[1]);
    while(getline(ifs,str)){
        commands.emplace_back(str);
    }


    ifs.close();
    solve(R,commands);
}
