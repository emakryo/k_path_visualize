#include<iostream>
#include<fstream>
#include<vector>
#include<list>
#include<set>
#include<map>
#include<string>
#include<algorithm>
#include<iterator>
#include<cstdlib>
using namespace std;

struct vertex{
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

map<int, vertex> parse_DIMACS(string filename){
  // parse from DIMACS format
  ifstream co_file(filename + ".co");
  ifstream gr_file(filename + ".gr");

  map<int, vertex> G;

  // read co file
  long nV, nE;

  char c;
  // skip until 'p' line
  while(co_file >> c, c != 'p') co_file.ignore(256, '\n');

  // read 'p' line
  char buf[10];
  co_file >> buf >> buf >> buf >> nV;

  for(int i=0; i<nV; i++){
    //skip until 'v' line
    while(co_file >> c, c != 'v') co_file.ignore(256, '\n');

    //read 'v' line
    int id;
    int x;
    int y;
    co_file >> id >> x >> y;

    G[id].x = x;
    G[id].y = y;
    G[id].E = list<int>();
    G[id].E_inv = list<int>();
  }

  // read gr file
  // skip until 'p' line
  while(gr_file >> c, c != 'p') gr_file.ignore(256, '\n');
  gr_file >> buf >> nV >> nE;

  //cout << nE << endl;

  for(long i=0; i<nE; i++){
    // skip until 'a' line
    while(gr_file >> c, c != 'a') gr_file.ignore(256, '\n');
    int from, to, weight;
    gr_file >> from >> to >> weight;
    G[from].E.push_back(to);
    G[to].E_inv.push_back(from);
  }

  return G;
}

map<int, vertex> parse_csv(string station, string join){
  ifstream station_file(station);
  ifstream join_file(join);

  map<int, vertex> G;
  map<pair<int, int>, int> co2id;
  map<int, int> identity;

  station_file.ignore(512, '\n');

  while(true){
    char buf[512];
    station_file.getline(buf,512,',');
    if(station_file.eof()) break;
    int id = atoi(buf);
    for(int i=0; i<8; i++) station_file.ignore(512, ',');
    station_file.getline(buf,512,',');
    int x = (int)(atof(buf)*1000000);
    station_file.getline(buf,512,',');
    int y = (int)(atof(buf)*1000000);
    if(co2id.find(make_pair(x,y)) == co2id.end()){ //not found
      co2id[make_pair(x,y)] = id;
      identity[id] = id;
      station_file.ignore(512,'\n');
      G[id].x = x;
      G[id].y = y;
      G[id].E = list<int>();
      G[id].E_inv = list<int>();
    }
    else{ // found
      identity[id] = co2id[make_pair(x,y)];
      station_file.ignore(512,'\n');
    }
  }

  join_file.ignore(512,'\n');

  while(true){
    char buf[512];
    join_file.ignore(512,',');
    if(join_file.eof()) break;
    join_file.getline(buf,512,',');
    int a = identity[atoi(buf)];
    join_file.getline(buf,512);
    int b = identity[atoi(buf)];

    if(G.find(a) == G.end() || G.find(b) == G.end()) continue;
    G[a].E.push_back(b);
    G[b].E.push_back(a);
    G[a].E_inv.push_back(b);
    G[b].E_inv.push_back(a);
  }

  return G;
}

map<int, vertex> clip(map<int, vertex> &G){
  map<int, vertex> H;

  int window_size[2] = {1000,600}; // x, y
  //int center_pos[2] = {40800000, -74000000}; // latitude, longitude NY road
  long long center_pos[2] = {35681391,139766103}; // latitude, longitude JP rail
  //long long longitude_range = 50000; //NY road
  long long longitude_range = 20000000; // JP rail
  long long latitude_range = (long long)((double)longitude_range *
                                         (double)window_size[1] /
                                         (double)window_size[0]);

  long long x_range[2] = {center_pos[1]-longitude_range/2,
                    center_pos[1]+longitude_range/2};
  long long y_range[2] = {center_pos[0]-latitude_range/2,
                    center_pos[0]+latitude_range/2};

  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    if(x_range[0] < i->second.x && i->second.x < x_range[1] &&
       y_range[0] < i->second.y && i->second.y < y_range[1]){
      vertex *v = &H[i->first], *w = &G[i->first];
      v->x = (w->x - x_range[0]) * window_size[0] / (x_range[1] - x_range[0]);
      v->y = (w->y - y_range[0]) * window_size[1] / (y_range[1] - y_range[0]);
      v->E = list<int>();
      v->E_inv = list<int>();
      for(list<int>::iterator j = w->E.begin();
          j != w->E.end(); j++){
        if(x_range[0] < G[*j].x && G[*j].x < x_range[1] &&
           y_range[0] < G[*j].y && G[*j].y < y_range[1]){
          v->E.push_back(*j);
        }
      }

      for(list<int>::iterator j = w->E_inv.begin();
          j != w->E_inv.end(); j++){
        if(x_range[0] < G[*j].x && G[*j].x < x_range[1] &&
           y_range[0] < G[*j].y && G[*j].y < y_range[1]){
          v->E_inv.push_back(*j);
        }
      }

    }
  }
  return H;
}

void output_DIMACS(string filename, map<int, vertex> &G){
  ofstream co_file(filename+".co");
  ofstream gr_file(filename+".gr");
  int nE = 0;

  co_file << "p aux sp co " << G.size() << endl;

  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    co_file << "v " << i->first << " "
            << i->second.x << " "
            << i->second.y << endl;
    nE += i->second.E.size();
  }

  gr_file << "p sp " << G.size() << " " << nE << endl;

  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    for(list<int>::iterator j = i->second.E.begin();
        j != i->second.E.end(); j++){
      gr_file << "a " << i->first << " " << *j << endl;
    }
  }

}

void output_json(string filename, map<int, vertex> &G){
  ofstream json_file(filename+".json");

  json_file << "nodes = {" << endl;

  json_file << "\"0\":{\"x\":0,\"y\":0}," << endl;

  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    json_file << "\"" << i->first << "\":{\"x\":" << i->second.x
              << ", \"y\":" << i->second.y << "}," << endl;
  }
  json_file << "\"length\":" << G.size() << "};" << endl;

  json_file << "arcs = [" << endl;

  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    for(list<int>::iterator j = i->second.E.begin();
        j != i->second.E.end(); j++){

      json_file << "{\"from\":" << i->first << ",\"to\":" << *j << "}," << endl;

    }
  }

  json_file << "{\"from\":0,\"to\":0}];" << endl;
}

bool node_necessary(int k, int v, set<int> &C, map<int, vertex> &V){

  list<list<int> > P(1, list<int>(1,v));

  // construct path set P

  list<list<int> > P_que(P); // Path Set s.t. its length = l+1

  while(!P_que.empty()){

    list<int> p = P_que.front();
    P_que.pop_front();

    for(list<int>::iterator n = V[p.back()].E.begin();
        n != V[p.back()].E.end(); n++){  // n : next node

      if(C.find(*n) != C.end()) continue; // C include n
      if(find(p.begin(), p.end(), *n) != p.end()) continue; // p include n

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
        if(C.find(*n) != C.end()) continue; // r include n;
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
  for(int i=0; i<(int)C.size(); i++){
    if(C[i]) cout << i << " ";
  }
  cout << endl;
}

bool cmp(int i, int j, map<int,vertex> &G){
  return G[i].E.size() > G[j].E.size(); // by degree
  //return i>j; // by id
}

void sort_rec(vector<int>::iterator s, vector<int>::iterator t, map<int, vertex> &G){
  if(next(s) == t) return;

  vector<int>::iterator u=s, i=next(s), j=prev(t);
  //cout << *u << " " << *i << " " << *j << endl;
  if(!cmp(*s,*i,G)){
    swap(*u,*i);
  }
  // *(s+1) ~ *(i-1) < *u
  // *i ~ *j >= *u
  while(i != j){
    if(cmp(*j,*u,G)) j--; // *j > *u
    else if(cmp(*u,*i,G)) i++; // *i < *u
    else{
      swap(*i,*j);
      j--;
    }
  }
  //cout << "*u " << *u << endl;
  swap(*u, *i);
  //for(u = s; u != t; u++)
  //    cout << *u << " ";
  //  cout << endl;
  sort_rec(s,i,G);
  sort_rec(i,t,G);
}

vector<int> sort(map<int, vertex> &G){
  vector<int> V(G.size());

  int n=0;
  for(map<int, vertex>::iterator i=G.begin();
      i != G.end(); i++, n++){
    V[n] = i->first;
  }

  sort_rec(V.begin(), V.end(), G);

  return V;
}

set<int> APC(int k, map<int, vertex> &G){
  set<int> C;

  for(map<int,vertex>::iterator i=G.begin();
      i != G.end(); i++){
    C.insert(i->first);
  }

  // prune by id
  /*
  for(map<int,vertex>::iterator i=G.begin();
      i != G.end(); i++){
    if(!node_necessary(k, i->first, C, G)) C.erase(i->first);
  }
  */

  vector<int> nodes = sort(G);
  for(int i=0; i<(int)nodes.size(); i++){
    if(!node_necessary(k, nodes[i], C, G)) C.erase(nodes[i]);
  }

  return C;
}

set<int> SPC_quick(int k, map<int, vertex> &G){
  set<int> C;

  for(map<int,vertex>::iterator i=G.begin();
      i != G.end(); i++){
    C.insert(i->first);
  }

  vector<int> nodes = sort(G);

  return C;
}

map<int, vertex> overlay_graph(set<int> &C, map<int, vertex> &V){
  map<int, vertex> H;

  for(set<int>::iterator i=C.begin();
      i!=C.end(); i++){

    list<list<int> > que(1, list<int>(1, *i));
    set<int> reached; // reached APC node
    H[*i].x = V[*i].x;
    H[*i].y = V[*i].y;

    while(!que.empty()){
      list<int> p = que.front();
      que.pop_front();

      for(list<int>::iterator j = V[p.back()].E.begin();
          j != V[p.back()].E.end(); j++){
        if(find(p.begin(), p.end(), *j) != p.end()) continue; // p include *j
        if(C.find(*j) != C.end()){ // reach APC node
          reached.insert(*j);
        }
        else{
          list<int> q(p);
          q.push_back(*j);
          que.push_back(q);
        }
      }
    }

    list<int> E;

    for(set<int>::iterator j=reached.begin();
        j != reached.end(); j++){
      E.push_back(*j);
    }
    H[*i].E = E;

  }
  return H;
}

void output_degree_distribute(string filename, map<int, vertex> &G){
  map<int, int> degree_distrebute;
  for(map<int, vertex>::iterator i = G.begin();
      i != G.end(); i++){
    degree_distrebute[i->second.E.size()] += 1;
  }

  ofstream file(filename);
  for(map<int, int>::iterator i = degree_distrebute.begin();
      i != degree_distrebute.end(); i++){
    file << i->first << " "  << i->second << endl;
  }
}

int main(){

  /*
  map<int, vertex> G = parse_DIMACS("USA-road-d.NY");
  map<int, vertex> H = clip(G);
  cout << H.size() << endl;
  output_degree_distribute("USA-road-d-degree-distribute.txt",H);
  //output_json("USA-road-d.NY-rand",H);
  //set<int> C = APC(8,H);
  //map<int, vertex> F = overlay_graph(C, H);
  //output_json("USA-road-d.NY-rand-id8", F);
  */


  map<int, vertex> G = parse_csv("station20150414free.csv", "join20150414.csv");
  map<int, vertex> H = clip(G);
  //output_json("JP-rail-Tokyo",H);
  output_degree_distribute("JP-rail-Tokyo-degree-distribute.txt",H);
  cout << H.size() << endl;
  //set<int> C = APC(4,H);
  //map<int, vertex> F = overlay_graph(C,H);
  //output_json("JP-rail-Tokyo-deg4",F);

  return 0;
}
