#ifndef _TREE_R_H
#define _TREE_R_H
#include <bits/stdc++.h>
#include "Point.hpp"
#define hash_map std::unordered_map
#define NINF -1000000
using namespace std;


class CoverTreeRec{
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
        CoverTreeNode* point;
    };

public:
    CoverTreeNode* _root;
    unsigned int _numNodes;
    int _maxLevel;
    int _minLevel;
    hash_map<int, int> _level_size;
    const double base = 2.0;

    CoverTreeRec()
    {
        _root=NULL;
        _numNodes=0;
        _maxLevel=0;
        _minLevel=0;
    }


    ~CoverTreeRec(){
        //cout << "Deleting tree" << endl;
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        for(int i = _maxLevel; i>=_minLevel; i--){
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                    //cout << " Delete " << p->toString() << endl;
                    delete p;
                }
                else{ // add itself
                    Q1.push_back(p);
                }
            }
            Q = Q1;
        }
        for(CoverTreeNode* p : Q){
            //cout << " Delete " << p->toString() << endl;
            delete p;
        }
    }

    void insert(const Point& newPoint)
    {
        if(_root==NULL) {
            _root = new CoverTreeNode(newPoint, 0);
            _root->weight = 1;
            _numNodes=1;
            return;
        }

        double rootDist = newPoint.distance(_root->p);
        if(rootDist == 0.0) {
            return;
        }
        int base_level = ceilf(log(rootDist)/log(base));

        if(_numNodes == 1) {
            _maxLevel = base_level+1;
            _minLevel = base_level-1;
            _root->level = _maxLevel;
            _root->weight = 2;
            _root->children = vector<CoverTreeNode*>();
            CoverTreeNode* bifurcation = new CoverTreeNode(_root->p, base_level);
            _root->addChild(bifurcation);
            bifurcation->addChild(new CoverTreeNode(newPoint, NINF));
            bifurcation->addChild(new CoverTreeNode(_root->p, NINF));
            bifurcation->weight = 2;

            _level_size[_maxLevel] = 1;
            _level_size[_maxLevel-1] = 1;
            _level_size[_minLevel] = 2;
            _numNodes++;
            return;
        }

        if(base_level+1 > _maxLevel) {
            for(int i=_maxLevel; i<= base_level+1; i++)
                _level_size[i] = 1;
            _maxLevel = base_level+1;
            _root->level = _maxLevel;
        }

        CoverTreeNode* found_parent = NULL;
        insert_rec(newPoint, vector<CoverSetData>(1, {rootDist, _root}), _maxLevel, found_parent);
    }

    bool insert_rec(const Point& p, const vector<CoverSetData>& Qi, int level, CoverTreeNode*& found_parent){
        if(level < _minLevel){
            _minLevel = level;
            _level_size[_minLevel] = _level_size[_minLevel+1];
        }
        vector<CoverSetData> Q;
        double sep = pow(base, level);
        for(const CoverSetData& cover_point : Qi){
            if(cover_point.point->level == level){
                for(CoverTreeNode* child : cover_point.point->children){
                    double d = p.distance(child->p);
                    if(d <= sep) {
                        if(d == 0.0) return true;
                        Q.push_back({d, child});
                    }
                }
            }
            else{ // add self child
                if(cover_point.dist <= sep)
                    Q.push_back(cover_point);
            }
        }
        if(Q.empty())
            return false;

        bool found = insert_rec(p, Q, level-1, found_parent);
        if(found){
            if(found_parent != NULL && found_parent->parent != NULL){
                found_parent->parent->weight++;
                found_parent = found_parent->parent;
            }
            return true;
        }

        CoverSetData best_cover = {DBL_MAX, NULL};
        for(const CoverSetData& cover_point : Qi){
            if(cover_point.dist < best_cover.dist)
                best_cover = cover_point;
        }
        if(best_cover.dist <= sep){
            if(best_cover.point->level == level){
                // parent already explicit
                best_cover.point->addChild(new CoverTreeNode(p, NINF));
                best_cover.point->level = level;
                best_cover.point->weight++;
                found_parent = best_cover.point;
            }
            else{
                // create explicit parent
                best_cover.point->parent->removeChild(best_cover.point);
                CoverTreeNode* newNode = new CoverTreeNode(best_cover.point->p, level);
                best_cover.point->parent->addChild(newNode);

                newNode->addChild(best_cover.point);
                newNode->addChild(new CoverTreeNode(p, NINF));
                newNode->weight = best_cover.point->weight+1;
                found_parent = newNode;
            }
            _numNodes++;
            for(int i=level-1; i>=_minLevel; i--)
                _level_size[i]++;
            return true;
        }
        else{
            return false;
        }

    }

    void remove(const Point& p){
        if(_root==NULL) return;
        if(_numNodes==1 && (p.distance(_root->p) == 0.0)) {
            delete _root;
            _numNodes--;
            _root=NULL;
            return;
        }
        vector<vector<CoverSetData>> coverSets(1);
        coverSets[0].push_back({_root->p.distance(p), _root});
        remove_rec(p, coverSets, _maxLevel);

        _root->weight = _numNodes;
        // update bounds
        if(_level_size[_maxLevel-1] > 1){
            _maxLevel++;
            _level_size[_maxLevel] = 1;
            CoverTreeNode* oldRoot = _root;
            _root = new CoverTreeNode(oldRoot->p, _maxLevel);
            _root->addChild(oldRoot);
            _root->weight = oldRoot->weight;
        }
        for(int i = _maxLevel; i>= _minLevel+2; i--){
            if(_level_size[i-2] == 1)
                _maxLevel--;
        }
        for(int i = _minLevel; i< _maxLevel; i++){
            if(_level_size[i+1] == _numNodes)
                _minLevel++;
        }
        _root->level = _maxLevel;
    }

    bool remove_rec(const Point& p, vector<vector<CoverSetData>>& coverSets, int level){
        vector<CoverSetData> Q;
        double sep = pow(base, level);
        CoverTreeNode* toRemove = NULL;
        for(const CoverSetData& cover_point : coverSets.back()){
            if(cover_point.point->level == level){
                vector<CoverTreeNode*> children = cover_point.point->children; //create copy
                for(CoverTreeNode* child : children){
                    double d = p.distance(child->p);
                    if(d == 0.0){
                        if(child->level != NINF && child->level != level-1){ // make it explicit
                            child->parent->removeChild(child);
                            CoverTreeNode* newNode = new CoverTreeNode(child->p, level-1);
                            child->parent->addChild(newNode);

                            newNode->addChild(child);
                            newNode->weight = child->weight;
                            Q.push_back({d, newNode});
                            toRemove = newNode;
                        }
                        else{
                            Q.push_back({d, child});
                            toRemove = child;
                        }
                    }
                    else if(d <= sep) {
                        Q.push_back( {d, child});
                    }
                }
            }
            else{ // add self child
                if(cover_point.dist <= sep)
                    Q.push_back(cover_point);
            }
        }
        coverSets.push_back(Q);

        bool found = false;
        if(level > _minLevel)
            found = remove_rec(p, coverSets, level-1);

        if(toRemove != NULL){
            if(toRemove->level == NINF){
                _numNodes--;
            }
            found = true;
            CoverTreeNode* parent = toRemove->parent;
            parent->removeChild(toRemove);

            for(int i=level-1; i>=toRemove->level && i>=_minLevel; i--){
                _level_size[i]--;
            }

            // remove toRemove from coverset
            for(int i=0;i<coverSets.back().size();i++) {
                if(coverSets.back()[i].point == toRemove) {
                    coverSets.back()[i] = coverSets.back().back();
                    coverSets.back().pop_back();
                    break;
                }
            }


            for(CoverTreeNode* child : toRemove->children){
                int i1 = level-1;
                int id = coverSets.size()-1;
                CoverSetData best;
                while(i1 <= _maxLevel){
                    best = {DBL_MAX, NULL};
                    for(const CoverSetData& cover_point : coverSets[id]){
                        double tmp_dist = cover_point.point->distance(child);
                        if(tmp_dist <= best.dist)
                            best = {tmp_dist, cover_point.point};
                    }
                    if(best.dist <= pow(base, i1))
                        break;
                    _level_size[i1]++;
                    coverSets[id].push_back({child->p.distance(p), child});
                    i1++; id--;
                }
                //cout << " assigned to " << best.point->toString() << endl;
                if(_root == parent){ // reached the root, use the lifted child as new root
                    CoverTreeNode* oldRoot = _root;
                    _root = new CoverTreeNode(child->p, toRemove->level+1);
                    _root->weight = child->weight;
                    _root->addChild(child);
                    double rootDist = p.distance(_root->p);
                    coverSets[0] = vector<CoverSetData>(1, {rootDist, _root});
                    coverSets[1] = vector<CoverSetData>(1, {rootDist, child});
                    delete oldRoot;
                }
                else{ // place lifted child
                    if(best.point->level == i1){
                        // parent already explicit
                        best.point->addChild(child);
                    }
                    else{
                        CoverTreeNode* newNode = new CoverTreeNode(best.point->p, best.point->level);
                        newNode->weight = best.point->weight;
                        for(CoverTreeNode* child : best.point->children)
                            newNode->addChild(child);

                        best.point->level = i1;
                        best.point->children = vector<CoverTreeNode*>(0);
                        best.point->addChild(newNode);
                        best.point->addChild(child);
                        for(int i=_maxLevel-(i1-1); i<=min((int)coverSets.size()-1, _maxLevel-newNode->level); i++){ // we have to update the coverset with the new node
                            for(int j=0; j<coverSets[i].size(); j++){
                                if(coverSets[i][j].point == best.point){
                                    coverSets[i][j].point = newNode;
                                    break;
                                }
                            }
                        }


                    }

                }
            }
            delete toRemove;

        }
        // update weights
        for(CoverSetData& q : coverSets.back()){
            if(q.point->level != level-1) continue;
            int size = 0;
            for(CoverTreeNode* child : q.point->children){
                size += child->weight;
            }
            q.point->weight = size;
        
            // evict nodes that lost children
            if(q.point->children.size() == 1){
                CoverTreeNode* self_child = q.point->children[0];
                q.point->level = self_child->level;
                q.point->children = vector<CoverTreeNode*>(0);
                for(CoverTreeNode* child : self_child->children)
                    q.point->addChild(child);
                delete self_child;
            }

        }

        coverSets.pop_back();
        return found;
    }

    vector<Point> getCoreset(int k, double eps){
        if(_root == NULL) return vector<Point>();
        vector<CoverTreeNode*> Q;
        Q.push_back(_root);
        int i = _maxLevel;
        for(; i>_minLevel; i--){
            if(_level_size[i-1] >= k+1) break;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                else{ // add itself
                    Q1.push_back(p);
                }
            }
            Q = Q1;
        }
        int drop = max(i-(int)ceil(log2(8.0/eps)), _minLevel);
        for(; i>drop; i--){
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                    }
                }
                else{ // add itself
                    Q1.push_back(p);
                }
            }
            Q = Q1;
        }
        vector<Point> coreset;
        for(CoverTreeNode* node : Q)
            coreset.push_back(node->p);
        return coreset;
    }



    void print(){
        cout << "Cover Tree: " << _numNodes << " nodes" << endl;
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        for(int i = _maxLevel; i>=_minLevel; i--){
            cout << "Level " << i <<"  size:" << _level_size[i] << endl;
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                cout << p->toString() << " -> [" ;
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        Q1.push_back(ch);
                        cout << " " << ch->toString();

                    }
                }
                else{ // add itself
                    cout << " "<< p->toString()<<"*";
                    Q1.push_back(p);
                }
                cout << " ]  weight: " << p->weight << endl;

            }
            Q = Q1;
        }
        cout << endl;
    }

    void invariants(){
        vector<CoverTreeNode*> Q;
        if(_root == NULL) return;
        Q.push_back(_root);
        for(int i = _maxLevel; i>=_minLevel; i--){
            assert(_numNodes < 2 || _level_size[i] == Q.size());
            for(CoverTreeNode* p1 : Q){
                for(CoverTreeNode* p2 : Q){
                    assert((p1==p2) || (p1->p.distance(p2->p) > pow(base, i)));
                }
            }
            vector<CoverTreeNode*> Q1;
            for(CoverTreeNode* p : Q){
                assert(p->weight == weight_dfs(p));
                if(p->level == i){
                    for(CoverTreeNode* ch : p->children){
                        assert(p->p.distance(ch->p) <= pow(base, i));
                        Q1.push_back(ch);
                    }
                }
                else{ // add itself
                    Q1.push_back(p);
                }
            }
            Q = Q1;
        }
    }

    int weight_dfs(CoverTreeNode* v){
        if(v->children.empty()) return 1;
        int size = 0;
        for(CoverTreeNode* ch : v->children)
            size += weight_dfs(ch);
        return size;
    }
    //*/

};

#endif
