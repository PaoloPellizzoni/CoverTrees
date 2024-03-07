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
    // usage: points queries k eps lim
    int dim = atoi(argv[6]);
    vector<Point> points;
    if(dim == 0) points = readPoints(argv[1], dim, atoi(argv[5])); 
    else points = readGeoPoints(argv[1], atoi(argv[5]));
    vector<pair<int, int>> queries = readQueries(argv[2]);
    
    int k = atoi(argv[3]);
    double eps = atof(argv[4]);
    int j = 0;
    
    unordered_set<int> curr_idx;
    
    for(pair<int, int> q : queries){
        if(q.first == 0){
            //cout << "Inserting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            curr_idx.insert(q.second);
            double tock = second();
            cerr << j++ << " 0 " <<tock-tick << endl;
        }
        if(q.first == 1){
            //cout << "Deleting " << q.second << " " << pt.toString() << endl;
            double tick = second();
            curr_idx.erase(q.second);
            double tock = second();
            cerr << j++ << " 1 " << tock-tick << endl;
        }
        if(q.first == 2){
            double tick = second();
            vector<Point> coreset;
            for(int idx : curr_idx) coreset.push_back(points[idx]);
            vector<Point> centers_gon = gonKCenter(coreset, k);
            double tock = second();
            cerr << j++ << " 2 " << tock-tick << endl;
            cout << "RESULT: ";
            for(Point pt : centers_gon){
                cout << pt.id << " ";
            }
            cout << endl;

        }
    }

    
}
