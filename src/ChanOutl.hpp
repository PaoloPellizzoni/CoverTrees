#ifndef _CHANO_H
#define _CHANO_H
#include <bits/stdc++.h>
#include "Point.hpp"

using namespace std;


struct PointSet{
	vector<int> ptr;
	vector<Point*> set;
	int elems;

	PointSet(int range){
		ptr = vector<int>(range+1, -1);
		set = vector<Point*>(range+1);
		elems = 0;
	}

	void insert(Point* p){
		if(ptr[p->id] > -1) return;
		set[elems] = p;
		ptr[p->id] = elems;
		elems++;
	}

	void erase(Point* p){
		int pos = ptr[p->id];
		if(pos < 0) return;
		set[pos] = set[elems-1];
		ptr[set[pos]->id] = pos;
		ptr[p->id] = -1;
		elems--;
	}

	int size(){
		return elems;
	}
	bool empty(){
		return (elems == 0);
	}
};

struct ChanOutl{
	int k, z;
	double minDist, maxDist;
	double eps;
	int nGuesses;
	int maxp;
	vector<int> l;
	vector<vector<PointSet>> C;
	vector<vector<Point*>> S;
	vector<PointSet> U;
	
	
	ChanOutl(double _eps, int _k, int _z, double _mD, double _MD, double _maxp){
		k = _k;
		z = _z;
		minDist = _mD;
		maxDist = _MD;
		eps = _eps;
		maxp = _maxp;
		nGuesses = (int)(log(maxDist/minDist)/log(1+eps) + 1);
		l = vector<int>(nGuesses, 0);
		S = vector<vector<Point*>>(nGuesses, vector<Point*>(k+z));
		C = vector<vector<PointSet>>(nGuesses,
					vector<PointSet>(k+z, PointSet(_maxp)));
		U = vector<PointSet>(nGuesses, PointSet(_maxp));

		srand(0);
	}
	
	vector<Point*> query(){
		for(int i=0; i<nGuesses; i++){
			if(U[i].empty()){
				vector<Point*> sol = gcluster(i, minDist*pow(1+eps, i), k);
				return sol;
			}
		}
		return vector<Point*>(0);
	}

	vector<Point*> gcluster(int i, double r, int k){
		vector<Point*> Cen = vector<Point*>();
		vector<int> W = vector<int>(l[i]);
		int cntW = W.size();

		while(Cen.size() < k && cntW > 0){
			int max_cntB = -1;
			int max_x = -1;
			for(int j=0; j < l[i]; j++){
				if(W[j] == 1) continue;
				Point* x = S[i][j];
				int cntB = 0;
				for(int jj=0; jj < l[i]; jj++){
					if(W[jj] == 1) continue;
					Point* y = S[i][jj];
					if(x->distance(*y) <= 6*r){
						cntB += C[i][jj].size();
					}
				}
				if(cntB > max_cntB){
					max_cntB = cntB;
					max_x = j;
				}
			}
			Cen.push_back(S[i][max_x]);

			for(int j=0; j < l[i]; j++){
				if(W[j] == 1) continue;
				if(S[i][max_x]->distance(*S[i][j]) <= 12*r){
					W[j] = 1;
					cntW--;
				}
			}
		}

		return Cen;
	}
	
    void insert(Point* p){
        double rho = minDist;
		for(int i=0; i<nGuesses; i++){
            insert_fast_r(p, i, 2*rho, k+z);
            rho *= (1+eps);
        }
    }

	/*
	void insert_r(Point* p, int i, double r, int t, int start){
		if(t == 0){
			U[i].insert(p);
			return;
		}
		
		int n = U[i].size()+1;
		for(int j=start; j<l[i]; j++) n += C[i][j].size();

		double rnd = rand() / (0.0+RAND_MAX);
		if(rnd < 1.0/n){
			vector<Point*> Xhat;
			PointSet C1hat = PointSet(maxp);
			C1hat.insert(p);
			for(int j=start; j<l[i]; j++){
				for(Point* q : C[i][j]){
					if(q->distance(*p) >= r) Xhat.push_back(q);
					else C1hat.insert(q);
				}
			}
			for(Point* q : U[i]){
				if(q->distance(*p) >= r) Xhat.push_back(q);
				else C1hat.insert(q);
			}

			vector<Point*> Shat = vector<Point*>(t-1);
			vector<PointSet> Chat = vector<PointSet>(t-1, PointSet(maxp));
			PointSet Uhat = PointSet(maxp);
			randReclust(Xhat, r, t-1, Shat, Chat, Uhat);

			S[i][start] = p;
			C[i][start] = C1hat;
			for(int j=0; j<Shat.size(); j++){
				S[i][start+1+j] = Shat[j];
				C[i][start+1+j] = Chat[j];
			}
			l[i] = start+1+Shat.size();
			U[i] = Uhat;
		}
		else{
			if(p->distance(*S[i][start]) <= r){
				C[i][start].insert(p);
			}
			else{
				insert_r(p, i, r, t-1, start+1);
			}
		}
		
	}
	*/

	void insert_fast_r(Point* p, int i, double r, int t){
		int j;
		for(j=0; j<l[i]; j++){
			//cout<< "d: "<< p->dist(*S[i][j]) << endl;
			if(p->distance(*S[i][j]) <= r){
				C[i][j].insert(p);
				break;
			}
		}
		if(j == l[i]){
			if(l[i] < t){
				S[i][l[i]] = p;
				C[i][l[i]].insert(p);
				l[i]++;
			}
			else{
				U[i].insert(p);
			}
		}
	}
	
	void remove(Point* p){
		double rho = minDist;
		for(int i=0; i<nGuesses; i++){
			remove_r(p, i, 2*rho, k+z);
			
			rho *= (1+eps);
		}
	}

	void remove_r(Point* p, int i, double r, int t){
		int ci = -1;
		//cout << "Guess "<< i << endl;
		for(int j=0; j<l[i]; j++){
			//cout << S[i][j].id << " "<< p.id << endl;
			if(S[i][j] == p){
				ci = j;
				break;
			}
		}
		//cout << ci << endl;
		if(ci == -1){ // not a center, easy
			for(int j=0; j<l[i]; j++){
				C[i][j].erase(p);
			}
			U[i].erase(p);
		}
		else{ // a center
			vector<Point*> Xhat;
			for(int j=ci; j<l[i]; j++){
				for(int iq=0; iq<C[i][j].size(); iq++){
					Point* q = C[i][j].set[iq]; //for(Point* q : C[i][j])
					Xhat.push_back(q);
				}
			}
			for(int iq=0; iq<U[i].size(); iq++){
				Point* q = U[i].set[iq]; //for(Point* q : U[i])
				Xhat.push_back(q);
			}
			
			vector<Point*> Shat = vector<Point*>(t-ci);
			vector<PointSet> Chat = vector<PointSet>(t-ci, PointSet(maxp));
			PointSet Uhat = PointSet(maxp);
			randReclust(Xhat, r, t-ci, Shat, Chat, Uhat);
			for(int j=0; j<Shat.size(); j++){
				S[i][ci+j] = Shat[j];
				C[i][ci+j] = Chat[j];
			}
			l[i] = ci+Shat.size();
			U[i] = Uhat;
			
		}
	}
	
	void randReclust(vector<Point*>& Xhat, double r, int K,
			vector<Point*>& Shat, vector<PointSet>& Chat, PointSet& Uhat){
		int j = 0;
		PointSet unc = PointSet(maxp);
		for(Point* p : Xhat) unc.insert(p);
		while(!unc.empty() && j < K){
			PointSet Cj = PointSet(maxp);
			int rnd = rand()%unc.size();
			Point* c = unc.set[rnd]; // random point
			
			for(int iq=0; iq<unc.size(); iq++){
				Point* p = unc.set[iq]; // for(Point* p : unc)
				if(c->distance(*p) < r)
					Cj.insert(p);
			}
			for(int iq=0; iq<Cj.size(); iq++){
				Point* p = Cj.set[iq]; // for(Point* p : Cj){
				unc.erase(p);
			}
			Shat[j] = c;
			Chat[j] = Cj;	
			j++;
		}
		Shat.resize(j);
		Uhat = unc;
	}
};

#endif