#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include "RTree.h"

struct Point {
  double coords[6];
};

typedef RTree<Point*, double, 6> Tree;

bool rangeCallback(Point* p) {
  return true;
}

double distance(const Point* a, const Point* b) {
  double sum = 0;
  for (int i = 0; i < 6; i++) {
    double d = a->coords[i] - b->coords[i];
    sum += d * d;
  }
  return std::sqrt(sum);
}

int main() {
  std::ifstream file("forest.txt");
  if (!file.is_open()) {
    std::cerr << "Error opening forest.txt\n";
    return 1;
  }

  Tree tree;
  std::vector<Point*> points;
  std::string line;
  size_t count = 0;

  clock_t start = clock();
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    Point* p = new Point();
    for (int i = 0; i < 6; i++) {
      std::string token;
      if (!std::getline(ss, token, ',')) break;
      p->coords[i] = std::stod(token);
    }
    points.push_back(p);
    tree.Insert(p->coords, p->coords, p);
    count++;
  }
  clock_t end = clock();
  file.close();

  double insertTime = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Inserted " << count << " points in " << insertTime << " seconds\n";

  double min[6], max[6];
  for (int i = 0; i < 6; i++) {
    // minimo pra range q uery.
    min[i] = 0.001;
    // max pra range
    max[i] = 0.4;
  }

  clock_t qStart = clock();
  int hits = tree.Search(min, max, rangeCallback);
  clock_t qEnd = clock();
  double queryTime = double(qEnd - qStart) / CLOCKS_PER_SEC;

  std::cout << "Range query -> " << hits << " results in " << queryTime << " seconds\n";

  Point queryPoint;
  // knn ta procurando um ponto: [0.4, 0.4, 0.4, 0.4, 0.4, 0.4].
  for (int i = 0; i < 6; i++) queryPoint.coords[i] = 0.4;

  int k = 5;
  std::vector<std::pair<double, Point*>> knn;
  knn.reserve(k);

  clock_t knnStart = clock();
  for (Point* p : points) {
    double dist = distance(&queryPoint, p);
    if (knn.size() < (size_t)k) {
      knn.push_back({dist, p});
    }
    else {
      double maxDist = 0;
      int idx = 0;
      for (int i = 0; i < k; i++) {
        if (knn[i].first > maxDist) {
          maxDist = knn[i].first;
          idx = i;
        }
      }
      if (dist < maxDist) knn[idx] = {dist, p};
    }
  }
  clock_t knnEnd = clock();
  double knnTime = double(knnEnd - knnStart) / CLOCKS_PER_SEC;

  std::cout << "KNN query (k=" << k << ") -> " << knn.size()
            << " results in " << knnTime << " seconds\n";

  for (auto &pair : knn) {
    // mostrando a distancia euclidiana entre o ponto buscado na knn e o ponto achado.
    std::cout << "dist=" << pair.first << "\n";

    // mostrnado o valor de cada dimensão do ponto achado.
    std::cout
      << pair.second->coords[0] << " | "
      << pair.second->coords[1] << " | "
      << pair.second->coords[2] << " | "
      << pair.second->coords[3] << " | "
      << pair.second->coords[4] << " | "
      << pair.second->coords[5] << " | "
      << "\n";
    
    std::cout << "------------" << "\n";
  }

  for (auto p : points) delete p;
  return 0;
}
