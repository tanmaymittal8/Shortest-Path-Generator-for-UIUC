/*
  application.cpp
  Project - Openstreet Maps
  Author: Tanmay Mittal
  Project discription: The file traverses the graphs and finds the closest meeting plce of two individuals and gives the total distance required to travel and the path with node ID's.
  Creative component: Finds the distance and outputs a path for a person to go from one place to another. It also has a functunality of adding a new stop on the trip and finding the total distance and path to reach both the destenations. The user should choose option c when prompted and can input any graph provided. Then the user is required to enter the starting and ending destination. After the path and distance is outputted the user can enter another destination to add on to the original destination and then the program will output the total distance and the path from the last destination to the new destination. To exit press #.
*/

// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//


#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"

using namespace std;
using namespace tinyxml2;
const double INF = numeric_limits<double>::max();  // global variable

class prioritize {
 public:
  bool operator()(
    const pair<long long, double>& p1,
    const pair<long long, double>& p2) const{
    return p1.second > p2.second;
  }
};

// set up a priority queue
priority_queue<
  pair<long long, double>,          // (key,value) pair
  vector<pair<long long, double>>,  // store pairs in a vector
  prioritize> unvisitedQueue;   // function object



class build_application {
  private:

    string person2Building;
    long long nearestNode1;
    long long nearestNode2;
    long long nearestNode1Center;
    map<long long, long long> predecessors1;
    map<long long, long long> predecessors2;
    map<long long, double> distances1;
    map<long long, double> distances2;
    vector<long long> printPath1;
    vector<long long> printPath2;
    set<string> buildings_left;
    BuildingInfo buildingCenter;

  public:
    map<long long, Coordinates>
        Nodes;
    vector<FootwayInfo> Footways;
    vector<BuildingInfo> Buildings;
    graph<long long, double> G;
    string filename;

    set<string> add_buildings_set1();
    void print_buildings1();
    BuildingInfo search_building(bool &found, string personBuilding);
    void print_init_location(BuildingInfo b1, BuildingInfo b2);
    long long nearest_node(BuildingInfo b);
    BuildingInfo nearest_building( Coordinates midpoint);
    vector<long long> get_path(map<long long, long long> predecessors, long long nearestNode2);
    void print_path(map<long long, double> distances1, map<long long, double> distances2, vector<long long> printPath1, vector<long long> printPath2, map<long long, long long> predecessors1, map<long long, long long> predecessors2, long long nearestNode1Center);
    void Dijkstra(long long nearestNode1, map<long long, long long> &predecessors, map<long long, double> &distances);

    void traverse(bool &found, string &person1Building, string &person2Building);

    build_application(map<long long, Coordinates> Nodes, vector<FootwayInfo> Footways, vector<BuildingInfo> Buildings, graph<long long, double> G, string filename)
    {
      this->Nodes = Nodes;
      this->Footways = Footways;
      this->Buildings = Buildings;
      this->G = G;
      this->filename = filename;
  }
};

// adds buildings to set
set<string> build_application::add_buildings_set1(){
    buildings_left.clear();  // resets the vector back to 0
    for (auto &y : Buildings) {  // adds all the buildings
        buildings_left.emplace(y.Fullname);
      }

      return buildings_left;
}

// prings out all the avaliable building for the user to see
void build_application::print_buildings1(){
  for (const auto& element : buildings_left) {
      cout << element << endl;
  }
}

// // searches if the building is avaliable
BuildingInfo build_application::search_building(bool &found, string personBuilding){
  
  for (auto p : Buildings) {
         if (personBuilding == p.Abbrev) {  // input mathches abbreb
           found = true;
           return p;  // return building
         } else if (p.Fullname.find(personBuilding) != string::npos) {
           found = true;
           return p;  // return building
    }  // checks, input matches fullname or part of it
  }
    found = false;  // if building not found
    return {};
}

void build_application::print_init_location(BuildingInfo b1,  BuildingInfo b2){
  
        cout << "Person 1's point:" << endl;
        cout << " " << b1.Fullname << " " <<  endl;
        cout << " (" << b1.Coords.Lat << ", " << b1.Coords.Lon << ")"<< endl;
        cout << "Person 2's point:" << endl;
        cout << " " << b2.Fullname << " " <<  endl;
        cout << " (" << b2.Coords.Lat << ", " << b2.Coords.Lon << ")"<< endl;

}

BuildingInfo build_application::nearest_building( Coordinates midpoint){
  
  double min = INF;
  BuildingInfo buildingCenter;
  for (auto &e : Buildings) {
    // checks if this building has already been ckecked and no path found
    if (buildings_left.count(e.Fullname) != 0) {
        double distance = distBetween2Points(midpoint.Lat,
                      midpoint.Lon, e.Coords.Lat, e.Coords.Lon);
        if (distance < min) {  // min algorithm
          min = distance;
          buildingCenter = e;  // save the min dist building
        }
      }
    }
  cout << " " << buildingCenter.Fullname << endl;
  cout << " (" << buildingCenter.Coords.Lat << ", ";
  cout << buildingCenter.Coords.Lon << ")" << endl;
  return buildingCenter;

}

long long build_application::nearest_node(BuildingInfo b){
  
  double min = INF;
  long long nearestNode;
  for (auto &e : Footways) {
     for (int i = 0; i < e.Nodes.size(); i++) {  // each node in footway
      double lat1 = Nodes.at(e.Nodes.at(i)).Lat;
      double lng1 = Nodes.at(e.Nodes.at(i)).Lon;
      double distance = distBetween2Points(lat1, lng1, b.Coords.Lat, b.Coords.Lon);
      if (distance < min) {  // min algorithm
         min = distance;
      nearestNode = Nodes.at(e.Nodes.at(i)).ID;  // save nearest node
     }
    }
  }
  cout << " " << nearestNode << endl;
  cout << " (" << Nodes.at(nearestNode).Lat << ", ";
  cout << Nodes.at(nearestNode).Lon << ")" << endl;
  return nearestNode;

}

vector<long long> build_application::get_path(map<long long, long long> predecessors, long long nearestNode2){
  
  long long currV;
  stack<long long> path;
  vector <long long> pathp;
  currV = nearestNode2;  // traverse backwards, dest
  while (currV != 0) {
    path.push(currV);
    // traverse the predecessor map until it reached 0
    currV = predecessors.at(currV);
  }
  // inverse direction of the input
  while (path.size() != 0) {
    currV = path.top();
    path.pop();
    pathp.push_back(currV);
  }
  return pathp;

}

void build_application::print_path(map<long long, double> distances1, map<long long, double> distances2,vector <long long> printPath1, vector <long long> printPath2, map<long long, long long> predecessors1, map<long long, long long> predecessors2, long long nearestNode1Center){

  cout << "Person 1's distance to dest: " << distances1.at(nearestNode1Center);
  cout << " miles" <<  endl;
  printPath1 = this->get_path(predecessors1, nearestNode1Center);
  cout << "Path: ";
  for (int i = 0; i < printPath1.size()-1; i++) {
    cout << printPath1.at(i) << "->";

  }
  cout << printPath1.at(printPath1.size()-1) << endl;
  cout << "Person 2's distance to dest: " << distances2.at(nearestNode1Center);
  cout << " miles" <<  endl;
  printPath2 = this->get_path(predecessors2, nearestNode1Center);
  cout << "Path: ";
  for (int c = 0; c < printPath2.size()-1; c++) {
    cout << printPath2.at(c) << "->";
  }
  cout << printPath2.at(printPath2.size()-1) << endl << endl;

}

void build_application::traverse(bool &found, string &person1Building, string &person2Building){
  BuildingInfo b1 = search_building(found, person1Building);

  if (found) {  // checks if b1 found in graph, or error statnm
    BuildingInfo b2;
    b2 = search_building( found, person2Building);
      if (found) {  // checks if b2 found in graph, or error statnm

          print_init_location(b1, b2);
          // locate center building
          Coordinates midpoint;
          midpoint = centerBetween2Points(b1.Coords.Lat, b1.Coords.Lon,
                                        b2.Coords.Lat, b2.Coords.Lon);
          cout << "Destination Building: " << endl;
          buildingCenter = nearest_building(midpoint);
          cout << endl;
          // FINDS NEAREST NODES FROM BUILDINGS 1,2 & CENTER
          
          cout << "Nearest P1 node:" << endl;
          nearestNode1 = this->nearest_node(b1);
          cout << "Nearest P2 node:" << endl;
          nearestNode2 = this->nearest_node(b2);
          cout << "Nearest destination node:" << endl;
          nearestNode1Center = this->nearest_node(buildingCenter);
      // RUN DIJKSTRA'S ALGORITHM
          this->Dijkstra(nearestNode1, predecessors1, distances1);
          this->Dijkstra(nearestNode2, predecessors2, distances2);
          if (distances1[nearestNode2] >= INF) {  // if no path possible
              cout << "Sorry, destination unreachable. " << endl;
            } else if (distances1[nearestNode1Center] >= INF || distances2[nearestNode1Center] >= INF) {  // find a different next closest path until no path is possibe
                while (buildings_left.size() == 0 || (distances1[nearestNode1Center] >= INF
                      && distances2[nearestNode1Center] >= INF)) {
            cout << "At least one person was unable to ";
            cout << "reach the destination building. ";
            cout << "Finding next closest building..." << endl;
            // keeps track of all the buildings tested
            buildings_left.erase(buildingCenter.Fullname);
            cout << "New destination building:" << endl;
            // recalculate center building
            buildingCenter = nearest_building(midpoint);
            cout << "Nearest destination node:" << endl;
            nearestNode1Center = this->nearest_node(buildingCenter);
          }
            this->print_path(distances1, distances2, printPath1, printPath2,
                  predecessors1, predecessors2, nearestNode1Center);
              } else {  // print path
              this->print_path(distances1, distances2, printPath1, printPath2,
                  predecessors1, predecessors2, nearestNode1Center);
              }
        } else { cout << "Person 2's building not found" << endl; }
      } else { cout << "Person 1's building not found" << endl; }

}

void build_application::Dijkstra(long long nearestNode1, map<long long, long long> &predecessors, map<long long, double> &distances){
  
    set <long long > visited1;
    vector<long long> allNodes = G.getVertices();
    for (auto &vertex : allNodes) {  // for each through every vertex
      distances[vertex] = INF;  // initial distance to infinity
      predecessors[vertex] = 0;  // initialize
      unvisitedQueue.push(make_pair(vertex, INF));  // push node to queue
    }
    distances[nearestNode1] = 0;  // initialize
    unvisitedQueue.push(make_pair(nearestNode1, 0));
    while (unvisitedQueue.size() != 0) {  // not empty
      pair<long long, double> currNode = unvisitedQueue.top();
      unvisitedQueue.pop();
      if (currNode.second == INF) {
      break;  // can't visit currntV
    } else if (visited1.count(currNode.first) != 0) {
        continue;  // currentV has been visited
    }  // visit current v
      visited1.insert(currNode.first);
       set<long long> neighbors = G.neighbors(currNode.first);
    for (auto &neighbor : neighbors) {
      double edgeWeight = 0;
      G.getWeight(currNode.first, neighbor, edgeWeight);
      double pathDist = currNode.second + edgeWeight;
      if (pathDist < distances[neighbor]) {
        distances[neighbor] = pathDist;
        predecessors[neighbor] = currNode.first;
        unvisitedQueue.push(make_pair(neighbor, pathDist));
      }  // set distance from start vertex to end for each vertex
    }  // predecessor array track path from each vertex
      }

}



int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;
  cout << "Choose from file name uic.osm or uiuc.osm" << endl;
  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);

  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


// adds vertices
// loops through the Nodes and adds all the verties in it
  graph <long long, double> G;
  for (auto &e : Nodes) {
    G.addVertex(e.first);
  }

// adds edges
  for (auto &t : Footways) {  // loops through footways
    // finds the nodes in each footway and finds the distance
    // btw two nodes
    for (long long i = 0; i < t.Nodes.size()-1; i++) {
      double lat1 = Nodes.at(t.Nodes.at(i)).Lat;
      double lng1 = Nodes.at(t.Nodes.at(i)).Lon;
      double lat2 = Nodes.at(t.Nodes.at(i+1)).Lat;
      double lng2 = Nodes.at(t.Nodes.at(i+1)).Lon;
      double distance = distBetween2Points(lat1, lng1, lat2, lng2);
      // adds the edges in both directions 
      G.addEdge(t.Nodes.at(i), t.Nodes.at(i+1), distance);
      G.addEdge(t.Nodes.at(i+1), t.Nodes.at(i), distance);
    }
  }
  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  // Menu
  string userInput;
  build_application journey(Nodes, Footways, Buildings, G, filename);
  cout << "Enter \"a\" for the standard application or "
        << "\"c\" for the creative component application> ";
  getline(cin, userInput);
  if (userInput == "a") {
    vector <long long> printPath1, printPath2;
    set <string> buildings_left;
    journey.add_buildings_set1();
    string person1Building, person2Building;
    cout << endl;
    journey.print_buildings1();
    
    cout << endl << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
    while (person1Building != "#") {
      cout << "Enter person 2's building (partial name or abbreviation)> ";
      getline(cin, person2Building);
      BuildingInfo b1, b2, buildingCenter;
      bool found = false;
      long long nearestNode1, nearestNode2, nearestNode1Center;
      map<long long, long long> predecessors1, predecessors2;
      map<long long, double> distances1, distances2;
      
      
    journey.traverse(found, person1Building, person2Building);
    journey.add_buildings_set1();  // reset buildings_left
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
      
    }
    
  } else if (userInput == "c") {
  // creative component
  cout << "try again";
  // creative(Nodes, Footways, Buildings, G);
  }
  cout << "** Done **" << endl;
  return 0;
}
