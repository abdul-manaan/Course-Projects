#include <iostream>

using namespace std;


void solve(){
    int R,C;
    cin >> R >> C;

    string box[R];
    for(int i =0; i< R; i++){
        cin >> box[i];
    }
    int row[R];
    int col[C];
    for(int i=0;i<R;i++){
        row[i] = 0;
    } 
    for(int i=0;i<C;i++){
        col[i] = 0;
    }

    for(int i=0;i<R;i++){
        for(int j=0; j<C;j++){
            
        }
    }
    
}
int main(){
    long long int testCases;
    cin >> testCases;

    for(long long i = 1; i <= testCases; i++)
    {
        cout << "Case #" << i << ": ";solve();
    }
    
}