#include <iostream>

using namespace std;

void solve(int F){
    int A[5];
    int B[5];
    int C[5];
    int D[5];
    int E[5];
    for(int i = 0; i < 5; i++)
    {
        A[i] = B[i] = C[i] = D[i] = E[i] = 0;
    }
    for(int i =1; i < 595; i+= 5){
        char s;
        cout << i << endl;
        cin >> s;
        if(s == 'A') A[0]++;
        else if(s == 'B') B[0]++;
        else if(s == 'C') C[0]++;
        else if(s == 'D') D[0]++;
        else if(s == 'E') E[0]++;
    }

    for(int i = 2; i < 595; i+=5){
        char s;
        cout << i << endl;
        cin >> s;
        if(s == 'A') A[1]++;
        if(s == 'B') B[1]++;
        if(s == 'C') C[1]++;
        if(s == 'D') D[1]++;
        if(s == 'E') E[1]++;        
    }

    for(int i = 3; i<595; i+=5){
        char s;
        cout << i << endl;
        cin >> s;
        if(s == 'A') A[2]++;
        if(s == 'B') B[2]++;
        if(s == 'C') C[2]++;
        if(s == 'D') D[2]++;
        if(s == 'E') E[2]++;        
    }
    
    for(int i = 9; i<=595; i+=5){
        char s;
        cout << i << endl;
        cin >> s;
        if(s == 'A') A[3]++;
        if(s == 'B') B[3]++;
        if(s == 'C') C[3]++;
        if(s == 'D') D[3]++;
        if(s == 'E') E[3]++;        
    }   

    string ans;

    bool a ,b,c,d,e;
    a = b= c= d = e = false;

    for(int i = 0; i < 4; i++){
        if(!a and A[i] < 24){
            a = true;
            ans += 'A';
        } else if(!b and B[i] < 24){
            b = true;
            ans += 'B';
        } else if(!c and C[i] < 24){
            c = true;
            ans += 'C';
        } else if(!d and D[i]< 24){
            d = true;
            ans += 'D';
        } else if(!e and E[i] < 24){
            e = true;
            ans += 'E';
        }
    }
    if(!a){
        ans += 'A';
    } else if(!b) {
        ans += 'B';
    } else if(!c){
        ans += 'C';
    } else if(!d){
        ans += 'D';
    } else  if(!e){
        ans += 'E';
    }

    cout << ans << endl;
    char anss;
    cin >> anss;
    if(anss == 'N'){
        exit(0);
    }
    return;
}
int main(){
    long long int testCases;
    cin >> testCases;
    int F;
    cin >> F;
    for(long long i = 1; i <= testCases; i++)
    {
        solve(F);
    }
    
}