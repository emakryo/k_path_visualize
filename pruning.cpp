#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
using namespace std;

struct vertex{
  int id;
  int x;
  int y;
  list<int> E;
  list<int> E_inv;
};

void print_path(list<int> p){
  for(list<int>::iterator i=p.begin();
      i!=p.end(); i++){
    cout << *i << " ";
  }
  cout << endl;
}

void input(vector<vertex> &V){
  int nV, nE;
  cin >> nV >> nE;
  V.assign(nV, (vertex){0, 0, 0, list<int>(), list<int>()});
  for(int i=0; i<nV; i++){
    V[i].id = i;
    V[i].x = 0;
    V[i].y = 0;
    V[i].E = list<int>();
    V[i].E_inv = list<int>();
  }
  for(int i=0; i<nE; i++){
    int s,t;
    cin >> s >> t;
    V[s].E.push_back(t);
    V[t].E_inv.push_back(s);
  }
}

bool node_necessary(int k, int v, vector<bool> &C, vector<vertex> &V){

  list<list<int> > P(1, list<int>(1,v));

  // construct path set P

  list<list<int> > P_que(P); // Path Set s.t. its length = l+1

  while(!P_que.empty()){

    list<int> p = P_que.front();
    P_que.pop_front();

    for(list<int>::iterator n = V[p.back()].E.begin();
        n != V[p.back()].E.end(); n++){  // n : next node

      if(find(p.begin(), p.end(), *n) != p.end()) continue; // p include n
      if(C[*n]) continue; // C include n

      if((int)p.size() ==  k-1) return true; // there is a k nodes path

      list<int> next_path(p);
      next_path.push_back(*n);
      P.push_back(next_path);
      P_que.push_back(next_path);
    }

  }

  /*for(list<list<int> >::iterator p = P.begin();
      p != P.end(); p++) print_path(*p);
  */

  for(list<list<int> >::iterator p=P.begin();
      p != P.end(); p++){

    list<list<int> > path_stack(1, list<int>(1,v));

    while(!path_stack.empty()){

      list<int> r = path_stack.front();
      path_stack.pop_front();

      for(list<int>::iterator n = V[r.back()].E_inv.begin();
          n != V[r.back()].E_inv.end(); n++){

        if(find(r.begin(), r.end(), *n) != r.end()) continue; // r include n
        if(find(p->begin(), p->end(), *n) != p->end()) continue; // p include n
        if(C[*n]) continue; // r include n;
        if((int)(p->size() + r.size()) == k) return true; // there is a k nodes path

        list<int> rn(r);
        rn.push_back(*n);
        path_stack.push_front(rn);
      }
    }
  }

  return false;
}

void print_cover(vector<bool> &C){
  for(int i=0; i<C.size(); i++){
    if(C[i]) cout << i << " ";
  }
  cout << endl;
}

vector<bool> APC(int k, vector<vertex> &V){
  vector<bool> C(V.size(), true);

  for(int i=0; i < V.size(); i++){
    if(!node_necessary(k, i, C, V)) C[i] = false;
  }

  return C;
}

int main(){
  vector<vertex> V;

  input(V);

  vector<bool> C = APC(4, V);
  print_cover(C);

  return 0;
}
