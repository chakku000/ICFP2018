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
const int dx[]={1,0,-1,0} ,dy[] = {0,1,0,-1};
/*}}}*/

void Prt(const string& s,int x,int y,int z){
    cout << s << " " << x << " " << y << " " << z << endl;
}


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

void solve(vector<string>& commands){
    int x=0,y=0,z=0;
    for(int i=0;i<commands.size();i++){
        auto command = split(commands[i],' ');
        if(command[0]=="SMove" or command[0]=="LMove"){
            x += stoi(command[1]);
            y += stoi(command[2]);
            z += stoi(command[3]);
        }else{
            int tx=1,ty=1,tz=1;
            if(x<0) tx=-1;
            if(y<0) ty=-1;
            if(z<0) tz=-1;
            x=abs(x);
            y=abs(y);
            z=abs(z);
            while(x){
                int nx = min(15,x);
                Prt("SMove",nx*tx,0,0);
                x -= nx;
            }
            while(y){
                int ny = min(15,y);
                Prt("SMove",0,ny*ty,0);
                y -= ny;
            }
            while(z){
                int nz = min(15,z);
                Prt("SMove",0,0,nz*tz);
                z -= nz;
            }

            cout << commands[i] << endl;
        }
    }
}

int main(int argc,char** argv){
    std::ifstream ifs(argv[1]);
    std::string str;
    vector<string> commands;

    if(argc==1){
        cerr << "引数氏〜〜" << endl;
        exit(1);
    }

    while(getline(ifs,str)){
        commands.emplace_back(str);
    }
    ifs.close();

    solve(commands);
}
