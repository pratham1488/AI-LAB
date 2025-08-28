#include <iostream>
#include <queue>
#include <set>
using namespace std;


#define CAP_A 4
#define CAP_B 3


struct State {
    int a, b;
    string path;
    State(int x, int y, string p) : a(x), b(y), path(p) {}
};


void waterJugBFS(int target) {
    set<pair<int, int>> visited;  
    queue<State> q;
    q.push(State(0, 0, ""));      

    while (!q.empty()) {
        State cur = q.front(); q.pop();

        
        if (cur.a == target || cur.b == target) {
            cout << "Solution Steps:\n" << cur.path << "\nFinal State: (" << cur.a << ", " << cur.b << ")\n";
            return;
        }

        
        if (visited.count({cur.a, cur.b})) continue;
        visited.insert({cur.a, cur.b});

        
        q.push(State(CAP_A, cur.b, cur.path + "Fill Jug A\n"));
        
        q.push(State(cur.a, CAP_B, cur.path + "Fill Jug B\n"));
        
        q.push(State(0, cur.b, cur.path + "Empty Jug A\n"));
        
        q.push(State(cur.a, 0, cur.path + "Empty Jug B\n"));
        
        int pourAB = min(cur.a, CAP_B - cur.b);
        q.push(State(cur.a - pourAB, cur.b + pourAB, cur.path + "Pour A to B\n"));
        
        int pourBA = min(cur.b, CAP_A - cur.a);
        q.push(State(cur.a + pourBA, cur.b - pourBA, cur.path + "Pour B to A\n"));
    }

    cout << "No solution found.\n";
}

int main() {
    int target = 2; 
    waterJugBFS(target);
    return 0;
}
 
