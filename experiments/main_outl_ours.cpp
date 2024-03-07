#include "../src/CoverTree.hpp"
#include "../src/utils.hpp"
#include "timekeeping.h"
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;

vector<Point> readGeoPoints(string filen, int maxn = -1){
    ifstream file;
    file.open(filen); 

    vector<Point> out;
    std::string line;
    int j=0;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        vector<double> pv(2);
        string tmp = "";
        ss >> tmp; ss >> pv[0]; ss >> pv[1];
        Point p(pv, j++);
        out.push_back(p);

        if(j == maxn) break;
    }

    file.close();
    return out;
}

vector<Point> readPoints(string filen, int d, int maxn = -1){
    ifstream file;
    file.open(filen); 

    vector<Point> out;
    std::string line;
    int j=0;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        vector<double> pv(d);
        for(int i=0; i<d; i++) ss >> pv[i];
        Point p(pv, j);
        out.push_back(p);
        if(++j == maxn) break;
    }

    file.close();
    return out;
}

vector<pair<int, int>> readQueries(string filen){
    ifstream file;
    file.open(filen); 

    vector<pair<int, int>> out;
    std::string line;
    int j=0;
    while (std::getline(file, line))
    {
        int q = 0, id = 0;
        if(line[0] == 'i'){
            q = 0;
            line[0] = ' ';
            std::istringstream ss(line);
            ss >> id;
        }
        if(line[0] == 'd'){
            q = 1;
            line[0] = ' ';
            std::istringstream ss(line);
            ss >> id;
        }
        if(line[0] == 'q'){
            q = 2;
        }
        pair<int, int> p = {q, id};
        out.push_back(p);
    }

    file.close();
    return out;
}


int main(int argc, char *argv[]) {
    CoverTree tree;
    // usage: points queries k z eps lim dim
    vector<Point> points;
    int dim = atoi(argv[7]);
    if(dim == 0) points = readGeoPoints(argv[1], atoi(argv[6]));
    else points = readPoints(argv[1], dim, atoi(argv[6]));
    vector<pair<int, int>> queries = readQueries(argv[2]);
    
    int k = atoi(argv[3]);
    int z = atoi(argv[4]);
    double eps = atof(argv[5]);
    int j = 0;
    
    
    for(pair<int, int> q : queries){
        if(q.first == 0){
            Point pt = points[q.second];
            //cout << "Inserting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            tree.insert(pt);
            double tock = second();
            cerr << j++ << " 0 " <<tock-tick << endl;
        }
        if(q.first == 1){
            Point pt = points[q.second];
            //cout << "Deleting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            tree.remove(pt);
            double tock = second();
            cerr << j++ << " 1 " << tock-tick << endl;
        }
        if(q.first == 2){
            double tick = second();
            vector<WeightedPoint> coreset = tree.getWeightedCoreset(k+z, eps);
            vector<vector<WeightedPoint>> sol = charikarOutliers(coreset, k, z, eps, 0.001, 400.0, 0);
            double tock = second();
            cerr << j++ << " 2 " << tock-tick << endl;
            cout << "RESULT: ";
            for(WeightedPoint pt : sol[0]){
                cout << pt.p.id << " ";
            }
            cout << endl;

        }
    }

    
}
