#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void ans(long long N, long long K){
    long long C[N], D[N];
    for(long long int i=0; i < N; i++){
        cin >> C[i];
    }
    for(long long int i=0; i < N; i++){
        cin >> D[i];
    }

    pair<long long, long long> grid[N][N];

    for(long long i = 0; i < N; i++){
        for(long long j = 0; j < N; j++){
            grid[i][j] = make_pair(0,0);
        }
    }
    
    
    grid[0][0] = make_pair(C[0],D[0]);
    vector<long long> ans;
    for(long long i = 0; i < N; i++){
        for(long long j = i; j< N; j++){
            if(i != j){
                auto prev = grid[i][j-1];
                grid[i][j] = make_pair( max(prev.first, C[j]), max(prev.second, D[j]));
                (grid[i][j].first > grid[i][j].second) ? 
                            ans.push_back(grid[i][j].first - grid[i][j].second) : 
                            ans.push_back( grid[i][j].second - grid[i][j].first);
            } else {
                grid[j][j] = make_pair(C[j],D[j]);
                (grid[i][j].first > grid[i][j].second) ? 
                            ans.push_back(grid[i][j].first - grid[i][j].second) : 
                            ans.push_back( grid[i][j].second - grid[i][j].first);    

            }
        }
    }

    long long int total = 0;
    for(long long int i=0; i< ans.size(); i++ ){
        if(ans[i] == K){
            total++;
        }
    }
    cout << total << endl; 
}
int main(){
    long long int testCases;
    cin >> testCases;
    for(long long int i = 1; i <= testCases; i++){
        long long int N,K;
        cin >> N >> K;
        cout << "Case #" << i << ": "; ans(N,K);
    }
}