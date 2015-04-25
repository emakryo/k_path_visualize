#include<iostream>
#include<fstream>
#include<set>
#include<cmath>
using namespace std;

int main(){

  ifstream co_file("USA-road-d.NY.co");

  int window_size[2] = {1000,600}; // x, y
  int center_pos[2] = {40768000, -73990000}; // latitude, longitude
  int longitude_range = 60000;
  int latitude_range = (int)((double)longitude_range * (double)window_size[1] /
                             (double)window_size[0]);

  int x_range[2] = {center_pos[1]-longitude_range/2,
                    center_pos[1]+longitude_range/2};
  int y_range[2] = {center_pos[0]-latitude_range/2,
                    center_pos[0]+latitude_range/2};

  set<int> select_nodes;

  ofstream new_co_file("USA-road-d.NY_central.co.json");
  new_co_file << "nodes = {" << endl;


  while(true){
    char c;
    co_file >> c;
    if(co_file.eof()) break;
    if(c == 'v'){
      int node;
      long long x, y;
      co_file >> node >> x >> y;
      if(x_range[0] < x && x < x_range[1] &&
         y_range[0] < y && y < y_range[1]){
        select_nodes.insert(node);
        new_co_file << "\"" << node << "\":{"
                    << "\"x\":"
                    << (x - x_range[0]) * window_size[0] / (x_range[1]-x_range[0])
                    << ","
                    << "\"y\":"
                    << (y - y_range[0]) * window_size[1] / (y_range[1]-y_range[0])
                    << "}," << endl;
      }
    }
    else{
      co_file.ignore(256, '\n');
    }
  }

  new_co_file << "\"length\":" << select_nodes.size() << "," << endl
              << "\"xRange\":[" << x_range[0] << "," << x_range[1] << "]," << endl
              << "\"yRange\":[" << y_range[0] << "," << y_range[1] << "]};" << endl;

  ifstream gr_file("USA-road-d.NY.gr");
  ofstream new_gr_file("USA-road-d.NY_central.gr.json");

  new_gr_file << "arcs = [" << endl;

  while(true){
    char c;
    gr_file >> c;
    if(gr_file.eof()) break;
    if(c == 'a'){
      int from, to, weigh;
      gr_file >> from >> to >> weigh;
      if(select_nodes.find(from) != select_nodes.end() &&
         select_nodes.find(to) != select_nodes.end()){
        new_gr_file << "{\"from\":" << from << ','
                    << "\"to\":" << to << ","
                    << "\"weigh\":" << weigh << "}," << endl;
      }
    }
    else{
      gr_file.ignore(256, '\n');
    }
  }

  new_gr_file << "];" << endl;

  return 0;

}
