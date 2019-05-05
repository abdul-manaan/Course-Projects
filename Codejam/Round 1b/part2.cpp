#include <iostream>

using namespace std;


void solve(int W){
    int ans[W];
    for(int i =0; i< W; i++){
        cout << i+1 << endl;
        cin >> ans[i];
    }
    
    int x1 = ans[5] - 2*ans[0];
    int x2 = 6*(ans[1]-ans[0]) + 2*(ans[2]-ans[1]);
    int a = (x1-x2)/40;
    int b = (ans[1]-ans[0]) - 2*a;
    
    int c = (ans[2]-ans[1])- 4*a;
    
    int d = (ans[3]-ans[2]) - 8*a - 2*b;
    
    int e = (ans[4]-ans[3]) - 16*a;
    
    int f = (ans[5]-ans[4]) - 32*a - 4*b - 2*c;
    
    cout << a << " " << 
            b << " " << 
            c << " " <<
            d << " " << 
            e << " " << 
            f << endl;
    int anser;
    cin >> anser;
    return;
}

int main(){
    int testCases;
    int W;
    cin >> testCases >> W;
    
    for(int i=1;i<=testCases; i++){
        solve(W);
    }
}