#ifndef _TREE_H
#define _TREE_H
#include <bits/stdc++.h>
#include "Point.hpp"

using namespace std;


class CoverTree{
    class CoverTreeNode{
    public:
        Point p;
        vector<CoverTreeNode*> children;
        CoverTreeNode* parent = NULL;
        int level;
        int weight;

        CoverTreeNode(const Point& _p, int _level){
            p = _p;
            level = _level;
            weight = 1;
        }

        ~CoverTreeNode(){
        }

        double distance(CoverTreeNode*& o) const{
            return p.distance(o->p);
        }

        void addChild(CoverTreeNode* p){
            p->parent = this;
            children.push_back(p);
        }

        void removeChild(CoverTreeNode* p){
            for(int i=0; i<children.size();i ++) {
                if(children[i] == p) {
                    children[i] = children.back();
                    children.pop_back();
                    break;
                }
            }
        }


        string toString(){
            return "( "+p.toString()+" "+to_string(level)+")";
        }

    };

    struct CoverSetData{
        double dist;
        CoverTreeNode* node;
    };

public:
    CoverTreeNode* _root;
    unsigned int _numNodes;
    const double base = 2.0;

    CoverTree()
    {
        _root=NULL;
        _numNodes=0;
    }


    ~CoverTree(){
        //cout << "Deleting tree" << endl;
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        for(int i = _root->level; ; i--){
            vector<CoverTreeNode*> Q1;
            bool minlevel = true;
            for(CoverTreeNode* p : Q){
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                    if(p->children.size() > 0) minlevel = false;
                    delete p;
                }
                else{ // add itself
                    if(p->children.size() > 0) minlevel = false;
                    Q1.push_back(p);
                }
                
            }
            Q = Q1;
            if(minlevel) break;
        }
    }

    void insert(const Point& newPoint)
    {
        if(_root==NULL) {
            _root = new CoverTreeNode(newPoint, 0);
            _root->weight = 1;
            _root->parent = NULL;
            _numNodes=1;
            return;
        }

        double rootDist = newPoint.distance(_root->p);
        int level;
        if(rootDist == 0.0) {
            return;
        }
        if( log(rootDist)/log(base) > _root->level ){
            level = ceilf(log(rootDist)/log(base));
            _root->level = level;
        }

        level = _root->level; 
        vector<vector<CoverSetData>> coverset(1);
        coverset.reserve(25);
        coverset[0].push_back({rootDist, _root});
        while(coverset[_root->level-level].size() > 0){
            double sep = pow(base, level);
            vector<CoverSetData> Qnew;
            for(const CoverSetData& cover_point : coverset[_root->level-level]){
                if(cover_point.node->children.size() == 0 || cover_point.node->children[0]->level != level-1){
                    if(cover_point.dist <= sep){
                        Qnew.push_back(cover_point);
                    }
                }
                else{
                    for(CoverTreeNode* child : cover_point.node->children){
                        double d = newPoint.distance(child->p);
                        if(d <= sep){
                            CoverSetData tmp; tmp.dist = d; tmp.node = child;
                            Qnew.push_back(tmp);
                        }
                    }
                }
            }
            coverset.push_back(Qnew);
            level--;
        }
        CoverTreeNode* v;
        while(true){
            double mindist = 1e20;
            for(CoverSetData cover_point : coverset[_root->level-(level+1)]){
                if(cover_point.dist < mindist){
                    mindist = cover_point.dist;
                    v = cover_point.node;
                }
            }
            if( mindist <= pow(base, level+1) ){
                break;
            }
            level++;
        }
        //cout << "inserting at level " << level << endl;
        CoverTreeNode* u = new CoverTreeNode(newPoint, level);

        if(v->children.size() == 0 || v->children[0]->level != level){
            CoverTreeNode* w = new CoverTreeNode(v->p, level);
            w->children = std::move(v->children); 
            w->weight = v->weight;
            for(CoverTreeNode* child : w->children){
                child->parent = w;
            }
            v->children = vector<CoverTreeNode*>(2);
            v->children[0] = u; u->parent = v;
            v->children[1] = w; w->parent = v;

        }
        else{
            v->children.push_back(u);
            u->parent = v;
        }

        // update weights
        CoverTreeNode* t = u->parent;
        while(t != NULL){
            t->weight++;
            t = t->parent;
        }

        _numNodes++;
    }


    void remove(const Point& p)
    {
        if(_root==NULL) return;
        if(_numNodes==1 && (p.distance(_root->p) == 0.0)) {
            delete _root;
            _numNodes--;
            _root=NULL;
            return;
        }
        
        int level = _root->level;
        vector<vector<CoverSetData>> coversets(1);
        coversets.reserve(25);
        coversets[0].push_back({_root->p.distance(p), _root});
        bool found = false;
        while(coversets[_root->level-level].size() > 0){
            double sep = pow(base, level);
            vector<CoverSetData> Qnew; 
            for(const CoverSetData& cover_point : coversets[_root->level-level]){
                if(cover_point.node->p.distance(p) == 0.0 && cover_point.node->children.size() == 0){
                    found = true;
                    break;
                }
                if(cover_point.node->children.size() == 0 || cover_point.node->children[0]->level != level-1){
                    if(cover_point.dist <= sep){
                        Qnew.push_back(cover_point);
                    }
                }
                else{
                    for(CoverTreeNode* child : cover_point.node->children){
                        double d = p.distance(child->p);
                        if(d <= sep){
                            CoverSetData tmp; tmp.dist = d; tmp.node = child;
                            Qnew.push_back(tmp);
                        }
                    }
                }
            }
            if(found) break;

            coversets.push_back(Qnew);
            level--;
        }

        
        vector<CoverTreeNode*> R; 
        while(level <= _root->level-1){
            for(CoverSetData& cover_point : coversets[_root->level-level]){
                if(cover_point.dist == 0.0 && cover_point.node->level == level){ // found node to delete
                    CoverTreeNode* todel = cover_point.node;
                    CoverTreeNode* v = todel->parent;
                    v->removeChild(todel);
                    delete todel;

                    if(v->p.distance(p) == 0.0){ // also v has to be removed
                        for(CoverTreeNode* child : v->children){
                            R.push_back(child);
                        }
                    }
                    else if(v->children.size() == 1){ // v now has only the self child
                        CoverTreeNode* selfchild = v->children[0];
                        v->children = std::move(selfchild->children);
                        for(CoverTreeNode* child : v->children){
                            child->parent = v;
                        }
                        v->weight = selfchild->weight;
                        delete selfchild;
                    }
                }
            }
            //cout << " deleting at level " << level << " R: "<<R.size()<< endl;
            vector<CoverTreeNode*> R_lp1;
            for(CoverTreeNode* w : R){
                CoverTreeNode* w2 = NULL;
                double sep = pow(base, level+1);
                for(CoverSetData& w3 : coversets[_root->level-(level+1)]){ // search in Q_{l+1}
                    if(w->distance(w3.node) <= sep && w3.node->p.distance(p) != 0.0){
                        w2 = w3.node;
                        break;
                    }
                }
                for(CoverTreeNode* w3 : R_lp1){ // search in R_{l+1}
                    if(w2 != NULL) break;
                    if(w->distance(w3) <= sep){
                        w2 = w3;
                        break;
                    }
                }

                if(w2 != NULL){
                    if(w2->children.size() > 0 && w2->children[0]->level == level){
                        w2->addChild(w);
                    }
                    else{
                        CoverTreeNode* z = new CoverTreeNode(w2->p, level);
                        z->children = std::move(w2->children);
                        z->weight = w2->weight;
                        for(CoverTreeNode* child : z->children){
                            child->parent = z;
                        }
                        w2->children = vector<CoverTreeNode*>(2);
                        w2->children[0] = z; z->parent = w2;
                        w2->children[1] = w; w->parent = w2;
                        w2->weight = z->weight + w->weight;
                    }
                }
                else{
                    w->level = level+1;
                    R_lp1.push_back(w);
                }
            }
            // update weights
            for(CoverSetData& w : coversets[_root->level-(level+1)]){
                w.node->weight = 0;
                if(w.node->children.size() == 0) w.node->weight = 1;
                for(CoverTreeNode* child : w.node->children)
                    w.node->weight += child->weight;
            }
            for(CoverTreeNode* w : R_lp1){
                w->weight = 0;
                if(w->children.size() == 0) w->weight = 1;
                for(CoverTreeNode* child : w->children)
                    w->weight += child->weight;
            }

            R = std::move(R_lp1); // update R
            coversets.pop_back();
            level++;
        }
        // handle root
        if(p.distance(_root->p) > 0.0){
            if(R.size() == 0){
                if(_root->children.size() > 0)
                    _root->level = _root->children[0]->level +1;
                else 
                    _root->level = 0;

                _root->weight = 0;
                for(CoverTreeNode* child : _root->children)
                    _root->weight += child->weight;
            }
            else{
                CoverTreeNode* newroot = new CoverTreeNode(_root->p, level+1);
                newroot->addChild(_root);
                for(CoverTreeNode* w : R){
                    newroot->addChild(w);
                }
                _root = newroot;
                _root->weight = 0;
                for(CoverTreeNode* child : _root->children)
                    _root->weight += child->weight;
            }
        }
        else{ // we are removing root
            if(R.size() == 1){
                _root = R[0];
                _root->parent = NULL;
                if(_root->children.size() > 0)
                    _root->level = _root->children[0]->level +1;
                else 
                    _root->level = 0;
                
                if(_root->children.size() > 0) _root->weight = 0;
                else _root->weight = 1;
                for(CoverTreeNode* child : _root->children)
                    _root->weight += child->weight;

            }
            else{
                CoverTreeNode* v = R[0];
                CoverTreeNode* newroot = new CoverTreeNode(v->p, level+1);
                for(CoverTreeNode* w : R){
                    newroot->addChild(w);
                }
                
                _root = newroot;
                _root->weight = 0;
                for(CoverTreeNode* child : _root->children)
                    _root->weight += child->weight;
            }
        }
        _numNodes--;
    }

    

    vector<Point> getCoreset(int k, double eps){
        if(_root == NULL) return vector<Point>();
        vector<CoverTreeNode*> Q;
        Q.push_back(_root);
        int i = _root->level;
        for( ; ; i--){
            bool minlevel = true;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                if(p->children.size() > 0) minlevel = false;
            }
            if(minlevel) break; // we are at minimum level 
            if(Q1.size() >= k+1) break;
            Q = Q1;
        }
        int dropped = i-(int)ceil(log2(8.0/eps));
        for(; i>dropped; i--){
            bool minlevel = true;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                if(p->children.size() > 0) minlevel = false;
            }
            if(minlevel) break; // we are at minimum level 
            Q = Q1;
        }
        vector<Point> coreset;
        for(CoverTreeNode* node : Q)
            coreset.push_back(node->p);
        return coreset;
    }


    vector<WeightedPoint> getWeightedCoreset(int k, double eps){
        if(_root == NULL) return vector<WeightedPoint>();
        vector<CoverTreeNode*> Q;
        Q.push_back(_root);
        int i = _root->level;
        for( ; ; i--){
            bool minlevel = true;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                if(p->children.size() > 0) minlevel = false;
            }
            if(minlevel) break; // we are at minimum level 
            if(Q1.size() >= k+1) break;
            Q = Q1;
        }
        int dropped = i-(int)ceil(log2(8.0/eps));
        for(; i>dropped; i--){
            bool minlevel = true;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                if(p->children.size() > 0) minlevel = false;
            }
            if(minlevel) break; // we are at minimum level 
            Q = Q1;
        }
        vector<WeightedPoint> coreset;
        for(CoverTreeNode* node : Q)
            coreset.push_back( {node->p, node->weight} );
        return coreset;
    }


    void print(){
        cout << "Cover Tree: " << _numNodes << " nodes" << endl;
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        for(int i = _root->level; ; i--){
            cout << "Level " << i <<"  size:" << Q.size() << endl;
            bool minlevel = true;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->level == i)
                    cout << p->toString() << " -> [" ;
                else 
                    cout << p->toString() << "* -> [" ;

                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    cout << " " << p->toString() << "*";
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                        cout << " " << ch->toString();
                    }
                }

                if(p->children.size() > 0) minlevel = false;

                cout << " ]  weight: " << p->weight << endl;

            }
            if(minlevel) break;
            Q = Q1;
        }
        cout << endl;
    }
    
    
    void invariants(){
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        bool ok = true;
        for(int i = _root->level; ; i--){
            bool minlevel = true;
            for(CoverTreeNode* p1 : Q){
                for(CoverTreeNode* p2 : Q){
                    assert((p1==p2) || (p1->p.distance(p2->p) > pow(base, i)));
                }
            }
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->weight != weight_dfs(p)) {cout << p->toString() << " " << p->weight << " " << weight_dfs(p) << endl; ok = false;}
                //assert(p->weight == weight_dfs(p));

                if(p->children.size() == 0 || p->children[0]->level != (i-1)   ){
                    Q1.push_back(p);
                }
                else{
                    for(CoverTreeNode* ch : p->children){
                        assert(p->p.distance(ch->p) <= pow(base, i));
                        Q1.push_back(ch);
                    }
                }
                if(p->children.size() > 0) minlevel = false;
            }
            if(minlevel) break;
            Q = Q1;
        }
        assert(ok);
    }

    int weight_dfs(CoverTreeNode* v){
        if(v->children.empty()) return 1;
        int size = 0;
        for(CoverTreeNode* ch : v->children)
            size += weight_dfs(ch);
        return size;
    }

};

#endif
