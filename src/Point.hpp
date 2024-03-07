#ifndef _POINT_H
#define _POINT_H
#include <bits/stdc++.h>
using namespace std;

class Point{
public:
    vector<double> v;
    int id;

    Point(vector<double> a, int idd){
        v = a;
        id = idd;
    }
    Point(vector<double> a){
        v = a;
        id = 0;
    }
    Point(){
    }

    double distance(const Point& o) const{
        double ans = 0;
        for(int i=0; i<v.size(); i++)
            ans += (v[i]-o.v[i])*((v[i]-o.v[i]));
        return sqrt(ans);
    }

    double sq_distance(const Point& o) const{
        double ans = 0;
        for(int i=0; i<v.size(); i++)
            ans += (v[i]-o.v[i])*((v[i]-o.v[i]));
        return ans;
    }

    string toString(){
        string s = "[";
        for(int i=0; i<v.size(); i++) s += to_string(v[i]) + " ";
        return s + "]";
    }
};


class WeightedPoint{
public: 
    Point p;
    int w;
};

#endif
