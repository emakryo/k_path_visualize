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

list<list<int> > construct_path_set(int k, int v, vector<bool> &C, vector<vertex> &V){
  list<list<int> > P(1, list<int>(1,v));
  list<list<int> > P_l(1, list<int>(1,v)); // Path Set s.t. its length = l+1

  for(int l=1; l<k; l++){
    list<list<int> > P_next;                   // Path Set s.t. its length = l+1

    for(list<list<int> >::iterator p = P_l.begin();
        p != P_l.end(); p++){  // p : Path from v

      list<int> *next_nodes = &(V[p->back()].E);

      for(list<int>::iterator n = next_nodes->begin();
          n != next_nodes->end(); n++){  // n : next node

        if(find(p->begin(), p->end(), *n) != p->end()) continue; // p include n
        if(C[*n]) continue; // C include n

        list<int> next_path(*p);
        next_path.push_back(*n);
        P_next.push_back(next_path);
      }
    }

    P_l = P_next;
    P.splice(P.end(), P_next);
  }

  return P;
}

bool node_necessary(int k, int v, vector<bool> &C, vector<vertex> &V){
  list<list<int> > P = construct_path_set(k,v,C,V);
  // Path Set s.t. its length = l

  return false;
}

vector<bool> APC(int k, vector<vertex> &V){
  vector<bool> C(V.size(), true);

  return C;
}

void print_path(list<int> p){
  for(list<int>::iterator i=p.begin();
      i!=p.end(); i++){
    cout << *i << " ";
  }
  cout << endl;
}

int main(){
  vector<vertex> V;

  input(V);

  for(vector<vertex>::iterator i=V.begin();
      i != V.end(); i++){
    for(list<int>::iterator j=i->E.begin();
        j != i->E.end(); j++){
      cout << i->id << " " << *j << endl;
    }
  }

  vector<bool> C(V.size(), false);
  list<list<int> > P = construct_path_set(4, 2, C, V);
  for(list<list<int> >::iterator p = P.begin();
      p != P.end(); p++) print_path(*p);

  return 0;
}
