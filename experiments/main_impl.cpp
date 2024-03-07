#include "../src/CoverTree.hpp"
#include "../src/CoverTreeRec.hpp"
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
    CoverTreeRec tree_rec;
    // usage: points queries k eps lim d
    vector<Point> points;
    int dim = atoi(argv[6]);
    if(dim == 0) points = readGeoPoints(argv[1], atoi(argv[5]));
    else points = readPoints(argv[1], dim, atoi(argv[5]));
    
    vector<pair<int, int>> queries = readQueries(argv[2]);
    
    int k = atoi(argv[3]);
    double eps = atof(argv[4]);
    int j = 0; 



    for(pair<int, int> q : queries){
        if(q.first == 0){
            Point pt = points[q.second];
            //cout << "Inserting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            tree.insert(pt);
            double tock = second();
            cerr << j++ << " 0 " << tock-tick << endl;
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
        }
    }

    
    for(pair<int, int> q : queries){
        if(q.first == 0){
            Point pt = points[q.second];
            //cout << "Inserting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            tree_rec.insert(pt);
            double tock = second();
            cerr << j++ << " 3 " << tock-tick << endl;
        }
        if(q.first == 1){
            Point pt = points[q.second];
            //cout << "Deleting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            tree_rec.remove(pt);
            double tock = second();
            cerr << j++ << " 4 " << tock-tick << endl;
        }
        if(q.first == 2){
        }
    }
    
    
    
}
