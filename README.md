
# OpenMap Nav: Optimal Meeting Point Finder

**A C++ navigation system that utilizes OpenStreetMap (OSM) data to calculate the optimal meeting location for two individuals and generates shortest-path walking routes.**


## Overview
This project is a graph-based navigation application designed to solve a "Social Rendezvous" problem. Given two starting locations on a university campus (or any mapped area), the application calculates the **geometric midpoint**, identifies the nearest accessible building to that center, and computes the shortest walking path for both individuals to meet there.

It parses raw XML data from OpenStreetMap to construct a weighted graph of nodes (intersections) and edges (footways), employing efficient pathfinding algorithms to ensure optimal routing.

## Key Features
* **OSM Data Parsing:** Utilizes `TinyXML2` to parse real-world map data (Nodes, Ways, Relations) from `.osm` files.
* **Graph Construction:** Dynamically builds a weighted undirected graph where nodes represent GPS coordinates and edges represent walking paths weighted by physical distance.
* **Optimal Meeting Logic:** Algorithms determine the geometric center between two arbitrary points and snap that coordinate to the nearest valid building from a dataset.
* **Shortest Path Navigation:** Implements **Dijkstra’s Algorithm** using a standard library Priority Queue to find the most efficient route.
* **Robust Error Handling:** Logic to handle unreachable destinations (e.g., disconnected graph components) by iteratively searching for the "next best" meeting location.

## Technical Implementation

### Data Structures & Algorithms
* **Graph Representation:** Adjacency list implementation (`graph` class) for memory efficiency on large maps.
* **Priority Queue:** Used a Min-Heap (`std::priority_queue` with custom comparator) to optimize Dijkstra's algorithm, reducing time complexity to **O(E + V log V)**.
* **STL Containers:** Extensive use of `std::map` for O(log n) lookups of Node IDs to Coordinates, and `std::vector` for storage of building and footway objects.

### Core Logic Flow
1. **Ingestion:** The system loads thousands of nodes and footways from `map.osm`.
2. **Triangulation:** * User inputs: `Building A` and `Building B`.
    * System calculates the geographic midpoint $(Lat_{mid}, Lon_{mid})$.
    * System queries the `Buildings` vector to find the structure with the minimum Euclidean distance to $(Lat_{mid}, Lon_{mid})$.
3. **Pathfinding:** * The system maps the physical buildings to their nearest "walkable" nodes on the footway graph.
    * Dijkstra's algorithm runs from both starting nodes to the destination node.
    * If a path is infinite (unreachable), the system gracefully degrades to the next nearest building.

## Getting Started

### Prerequisites
* C++ Compiler (GCC/Clang) supporting C++11 or higher.
* Make (optional, for build automation).

### Compiling
Run running_test1.exe to execute the program
