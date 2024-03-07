#include "../src/ChanOutl.hpp"
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
    // usage: points queries k z eps mD MD lim dim
    int dim = atoi(argv[9]);
    vector<Point> points;
    if(dim == 0) points = readGeoPoints(argv[1], atoi(argv[8]));
    else points = readPoints(argv[1], dim, atoi(argv[8]));

    vector<pair<int, int>> queries = readQueries(argv[2]);
    
    int k = atoi(argv[3]);
    int z = atoi(argv[4]);
    double eps = atof(argv[5]);

    double mD = atof(argv[6]);
    double MD = atof(argv[7]);

    ChanOutl chan = ChanOutl(eps, k, z, mD, MD, atoi(argv[8]));

    int j = 0;
    for(pair<int, int> q : queries){
        if(q.first == 0){
            double tick = second();
            chan.insert(&points[q.second]);
            double tock = second();
            cerr << j++ << " 0 " <<tock-tick << endl;
        }
        if(q.first == 1){
            double tick = second();
            chan.remove(&points[q.second]);
            double tock = second();
            cerr << j++ << " 1 " << tock-tick << endl;
        }
        if(q.first == 2){
            double tick = second();
            vector<Point*> sol = chan.query();
            double tock = second();
            cerr << j++ << " 2 " << tock-tick << endl;
            cout << "RESULT: ";
            for(Point* pt : sol){
                cout << pt->id << " ";
            }
            cout << endl;

        }
    }
    
}
