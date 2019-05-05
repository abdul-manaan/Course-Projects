#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int makeEqual(string C[], long long int n){
    long long int max = 0;
    for(long long int i = 0; i < n; i++)
    {
        if(C[i].length() > max) max = C[i].length();
    }
    max++;
    for(long long int i = 0; i < n; i++)
    {
        while(C[i].length() < max){
            C[i] = C[i]+C[i];
        }
        if(C[i].length() > max){
            C[i] = C[i].substr(0,max);
        }
    }
    return max;
}
void solve(){
    long long int N;
    cin >> N;
    string C[N];
    for(long long int i=0; i< N; i++){
        cin >> C[i];
    }
    long long int max = makeEqual(C,N);
    string answerStr;

    vector<string> ans;
    for(long long int i =0; i< N; i++){
        ans.push_back(C[i]);
    }

    for(long long int i = 0; i < max; i++){
        long long int R = 0;
        long long int P = 0;
        long long int S = 0;
        for(long long int j = 0; j < ans.size(); j++){
            if(ans[j][i] == 'R'){
                R = 1;
            } else if(ans[j][i] == 'P'){
                P =1;
            } else {
                S = 1;
            }
        }
        if(R == 1 and (P == 1 and S == 1)) {
            cout << "IMPOSSIBLE" << endl;
            return;
        } else if(R and S){
            answerStr += 'R';
            vector<string> toRemove; 
            for(long long int k = 0; k < ans.size(); k++){
                if(ans[k][i] == 'S'){
                    toRemove.push_back(ans[k]);
                }
            }
            for(long long int k = 0; k < toRemove.size(); k++){
                auto iter = find(ans.begin(), ans.end(), toRemove[k]);
                if(iter != ans.end()) ans.erase(iter);
            }
        } else if(R and P){
            answerStr += 'P';
            vector<string> toRemove; 
            for(long long int k = 0; k < ans.size(); k++){
                if(ans[k][i] == 'R'){
                    toRemove.push_back(ans[k]);
                }
            }
            for(long long int k = 0; k < toRemove.size(); k++){
                auto iter = find(ans.begin(), ans.end(), toRemove[k]);
                if(iter != ans.end()) ans.erase(iter);
            }
        }else if(S and P){
            answerStr += 'S';
            vector<string> toRemove; 
            for(long long int k = 0; k < ans.size(); k++){
                if(ans[k][i] == 'P'){
                    toRemove.push_back(ans[k]);
                }
            }
            for(long long int k = 0; k < toRemove.size(); k++){
                auto iter = find(ans.begin(), ans.end(), toRemove[k]);
                if(iter != ans.end()) ans.erase(iter);
            }
        } else if(R){
            answerStr += 'P';
            cout << answerStr << endl;return;
        } else if(S){
            answerStr += 'R';
            cout << answerStr << endl;return;
        } else{
            answerStr += 'S';
            cout << answerStr << endl; return;
        }
    }
    cout << answerStr << endl;
}


int main(){
    long long int testCases;
    cin >> testCases;

    for(long long i = 1; i <= testCases; i++)
    {
        cout << "Case #" << i << ": ";solve();
    }

    
}