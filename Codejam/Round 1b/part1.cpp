#include <iostream>

using namespace std;

void ans(long long P, long long Q){
    int grid[Q+1][Q+1];

    for(long long int i =0; i <= Q; i++){
        for(long long j = 0; j <= Q; j++)
        {
            grid[i][j] = 0;
        }
    }
    for(int i=0; i< P; i++){
        int x,y;
        char D;
        cin >> x >> y >> D;
        if(D == 'N'){
            for(long long z = 0; z <= Q; z++)
            {
                for(long long w = y+1; w <= Q; w++){
                    grid[z][w] += 1;
                }
            }
        } else if(D == 'S'){
            for(long long z = 0; z <= Q; z++)
            {
                for(long long w = 0; w < y; w++){
                    grid[z][w] += 1;
                }
            }
        } else if(D == 'E'){
            for(long long z = x+1; z <= Q; z++)
            {
                for(long long w = 0; w <= Q; w++){
                    grid[z][w] += 1;
                }
            }
        } else {
            for(long long z = 0; z <= x; z++)
            {
                for(long long w = 0; w < Q; w++){
                    grid[z][w] += 1;
                }
            }
        }
    }

    long long int ansX =0;
    long long int ansY =0;
    int cur =0;
    for(long long j = 0; j <= Q; j++)
        for(long long int i =0; i <= Q; i++){
        {
            if(grid[i][j] > cur){
                ansX = i;
                ansY = j;
                cur = grid[i][j];
            }
        }
    }
    cout << ansX << " " << ansY << endl;
}

int main(){
    long long int testCases;
    cin >> testCases;
    for(long long int i = 1; i <= testCases; i++){
        long long int P,Q;
        cin >> P >> Q;
        cout << "Case #" << i << ": ";ans(P,Q);
    }
}