void DijkstraShortestPath(graph<long long, double> G, long long nearestNode1, map<long long, long long> &predecessors, map<long long, double> &distances) {
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