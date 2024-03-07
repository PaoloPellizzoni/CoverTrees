#include <bits/stdc++.h>
#include "Point.hpp"
using namespace std;

double distanceBetweenSets(vector<Point> set, vector<Point> centers){
    double ans = 0;
    for(Point& p : set){
        double tmp = 1e20;
        for(Point& q : centers)
            tmp = min(tmp, p.distance(q));
        ans = max(ans, tmp);
    }
    return ans;
}

vector<Point> gonKCenter(vector<Point> points, int k){
    int n = points.size();
    if(n <= k) return points;
    vector<Point> sol;
    vector<double> distances(n, 1e20);
    int maxi = 0;
    for(int i=0; i < k; i++){
        sol.push_back(points[maxi]);
        double maxd = 0.0;
        for(int j=0; j < n; j++){
            distances[j] = min(distances[j], sol[i].distance(points[j]));
            if(distances[j] > maxd){
                maxd = distances[j];
                maxi = j;
            }
        }
    }
    return sol;
}





vector<vector<WeightedPoint>> charikarOutliers_r(vector<WeightedPoint> points, int k, int z, double eps, double r, double sample){
    int n = points.size();
    if(n < k){
        vector<vector<WeightedPoint>> sol = vector<vector<WeightedPoint>>(2);
        sol[0] = points;
        return sol;
    }
    if(sample == 0 || sample > n) sample = n;
    int cnt = n;
    vector<int> Tf = vector<int>(n);
    vector<WeightedPoint> X;
    double thr_1 = (1 + 2*eps)*r;
    double thr_2 = (3 + 4*eps)*r;

    vector<int> random_perm(n);
    iota(random_perm.begin(), random_perm.end(), 0);
    random_shuffle(random_perm.begin(), random_perm.end()); 
    
    while(X.size() < k && cnt > 0){
        int besti = 0;
        int best = 0;
        for(int i=0; i<n; i++){
            int tmp = 0;
            if(Tf[i]==0) tmp += points[i].w; // new 
            for(int jj=0; jj<sample; jj++){
                int j = random_perm[jj];
                if (i == j) continue; // new
                if(Tf[j]==0 && points[i].p.distance(points[j].p) <= thr_1)
                    tmp += points[j].w;
            }
            if(tmp > best){
                best = tmp;
                besti = i;
            }
        }
        X.push_back(points[besti]);
        for(int j=0; j<n; j++){
            if(Tf[j]==0 && points[besti].p.distance(points[j].p) <= thr_2){
                Tf[j] = 1;
                cnt--;
            }
        }
    }
    vector<vector<WeightedPoint>> sol = vector<vector<WeightedPoint>>(2);
    vector<WeightedPoint> Tff;
    for(int i=0; i<n; i++){
        if(Tf[i]==0) Tff.push_back(points[i]);
    }
    sol[0] = X; // centers
    sol[1] = Tff; //outliers
    return sol;
}

vector<vector<WeightedPoint>> charikarOutliers(vector<WeightedPoint> points, int k, int z, double eps, double mD, double MD, int sample, double beta=0.05){
    vector<double> Gamma = vector<double>((int)(log(MD/mD)/log(1+beta) + 1));
    int i = 0;
    for(double gamma = mD; i < Gamma.size(); gamma*=(1+beta)){
        Gamma[i] = gamma;
        i++;
    }

    vector<vector<WeightedPoint>> sol;
    // binary search
    int first = 0;
    int last = Gamma.size();
    while (first < last) {
        i = first + ((last - first) >> 1);

        sol = charikarOutliers_r(points, k, z, eps, Gamma[i], sample);
        vector<WeightedPoint> outl = sol[1];
        int tot = 0;
        for(WeightedPoint pp : outl){
            tot += pp.w;
        }
        if(tot > z)
            first = i + 1;
        else
            last = i;
    }
    sol = charikarOutliers_r(points, k, z, eps, Gamma[first], sample);
    return sol;
}



double distanceBetweenSetsOutliers(vector<Point> pset, vector<Point> centers, int z){
    if(pset.size() == 0 || centers.size()==0) return 0.0;
    vector<double> d = vector<double>(pset.size());
    for(int i=0; i<pset.size(); i++){
        Point p = pset[i];
        double dd = 1e20;
        for(Point q : centers){
            dd = min(dd, p.distance(q));
        }
        d[i] = dd;
    }
    sort(d.begin(), d.end());
    return d[pset.size() - 1 -z];
}