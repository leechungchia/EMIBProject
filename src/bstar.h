#ifndef BSTAR_H
#define BSTAR_H

#include <map>
#include <unordered_map>
#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>
#include "object.h"
#include "horizontal_contour.h"
#include "TCG.h"

using namespace std;

class BstarNode;
class CommonBstarPin;
class BstarTree;

class BstarNode: public rectangle
{
public:
    BstarNode(){};
    BstarNode(string t_code_name, vector<vector<TCG*>>& t_ecg){
        for(int i=0; i<t_ecg.size(); ++i){
            general_node* root;
            vector<general_node*> vec;
            topology_transfer(t_ecg[i], 0, 3, root, vec);
            root_record.push_back(root);
            node_record.push_back(vec);
            is_ecg = true;
        }
        current_num = make_pair(0,0);
        current_node = node_record[0][0];
    }
    BstarNode(string t_code_name, float t_width, float t_height):
    rectangle(t_code_name, t_width, t_height), m_is_visited(false){
        m_backup = new BstarNode();
        m_best = new BstarNode();
        is_ecg = false;
    }
    void               topology_transfer(vector<TCG*>& t_target, int t_root, int t_branch_num, general_node*& t_rootnode, vector<general_node*>& t_nodevec);
    int                rcounter(){return m_rcounter;};
    bool               is_visited(){return m_is_visited;};
    BstarNode*         left_child(){return m_left_child;};
    BstarNode*         right_child(){return m_right_child;};
    BstarNode*         parent(){return m_parent;};
    void               set_rcounter(int t_rcounter){m_rcounter = t_rcounter;};
    void               set_left_child(BstarNode* t_node){m_left_child = t_node;};
    void               set_right_child(BstarNode* t_node){m_right_child = t_node;};
    void               set_parent(BstarNode* t_node){m_parent = t_node;};
    void               set_visited(bool t_is_visited){m_is_visited = t_is_visited;};
    void               set_backup(BstarNode* t_node);
    void               set_best(BstarNode* t_node);
    BstarNode*         best(){return m_best;};
    BstarNode*         backup(){return m_backup;};
    void               do_backup();
    void               restore_best();
    void               get_backup();
    void               get_best();
    bool is_ecg;
    vector<vector<TCG*>>  topology_set;
    TCG*                  current_topology;
    general_node*         current_node;
    vector<vector<general_node*>> node_record;
    vector<general_node*>         root_record;
    int                   InitialIndex;
    pair<int,int>  back_num;
    pair<int,int>  current_num;
    pair<int,int>  best_num;
private:
    bool  m_is_visited;
    int   m_rcounter;
    BstarNode* m_left_child;
    BstarNode* m_right_child;
    BstarNode* m_parent;
    BstarNode* m_backup;
    BstarNode* m_best;
    void  m_copy(BstarNode* passive, BstarNode* active);
};

class CommonBstarPin: public pin{
    public:
        CommonBstarPin(string t_code_name, float t_width_diff, float t_height_diff):
        pin(t_code_name, t_width_diff, t_height_diff){}
        float              x(){
            return pin_xcoor(m_die_pointer->at(m_dual_die).first.first, m_die_ratio->at(m_dual_die).first, m_die_ratio->at(m_dual_die).second, m_die_pointer->at(m_dual_die).second);
        }
        float              y(){
            return pin_ycoor(m_die_pointer->at(m_dual_die).first.second, m_die_ratio->at(m_dual_die).first, m_die_ratio->at(m_dual_die).second, m_die_pointer->at(m_dual_die).second);
        }
        CommonBstarPin*         dual_pin(){return m_dual_pin;};
        void                    set_dual_pin(CommonBstarPin* t_dual_pin){m_dual_pin = t_dual_pin;};
        void                    set_dual_die(int t_index){m_dual_die = t_index;};
        void                    set_die_pointer(vector<pair<pair<float, float>, int>>* t_pointer){m_die_pointer = t_pointer;};
        void                    set_die_ratio(vector<pair<float, float>>* t_pointer){m_die_ratio = t_pointer;};
    private:
        vector<pair<pair<float, float>, int>>* m_die_pointer;
        vector<pair<float, float>>* m_die_ratio;
        CommonBstarPin*         m_dual_pin;
        int                     m_dual_die;
};

class BstarTree
{
public:
    /*BstarTree(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap)
    {
        m_TransformToBstarTree(t_NodeVec, t_PinVec, t_PinNodeMap);
        m_contour = new horizontal_contour;
        m_ground  = new BstarNode("ground",0,0);
        m_xy_reset();
        m_initialize();
        do_backups();
        restore_bests();
    }*/
    BstarTree(vector<pair<float, float>> t_die_inf, vector<pair<float, float>> t_DieVec, 
                vector<vector<vector<TCG*>>>& t_ecgnode, 
                vector<pair<pair<float, float>, pair<float, float>>> t_CommonNetVec,
                vector<pair<int, int>> t_MappingCommonPinToDie)
    {
        die_coors.resize(t_die_inf.size());
        for(int i=0; i<t_die_inf.size(); ++i){
            die_ratios.push_back(make_pair(t_die_inf[i].first, t_die_inf[i].second));
        }
        m_TransformToBstarTree(t_DieVec, t_ecgnode, t_CommonNetVec, t_MappingCommonPinToDie);
        m_contour = new horizontal_contour;
        m_ground  = new BstarNode("ground",0,0);
        m_xy_reset();
        cout << "instance created" << endl;
        m_initialize();
        cout << "initialized" << endl;
        do_backups();
        restore_bests();
        m_tf_seed     = 270;

    }
    void swap_move(BstarNode* node_1, BstarNode* node_2);
    void rotate(BstarNode* t_node);
    void delete_insert(BstarNode* d_node, BstarNode* i_node, bool dim);
    void transfer_topology(BstarNode* t_node, int t_change_group, bool t_dir, int t_branch);
    void do_backups();
    void restore_bests();
    void get_backups();
    void get_bests();
    float max_x(){return m_max_x;}
    float max_y(){return m_max_y;}
    float area();
    float HPWL();
    float best_max_x(){return m_max_x_best;}
    float best_max_y(){return m_max_y_best;}
    void m_get_coordinates();
    vector<float> get_dies_coor(int t_die_index);
    void write_output(char* arg);
    vector<BstarNode*>* nodes(){return &m_nodes;};
    vector<pair<pair<float, float>, int>> die_coors;
    vector<pair<float, float>>            die_ratios;
    vector<BstarNode*>  m_nodes;
    vector<BstarNode*>  m_ecgs;
private:
    horizontal_contour* m_contour;
    BstarNode*               m_ground;
    BstarNode*               m_root;
    BstarNode*               m_root_backup;
    BstarNode*               m_root_best;

    vector<pair<CommonBstarPin*, CommonBstarPin*>> m_common_nets;
    unordered_map<int, BstarNode*> m_die_map;
    float               m_max_x;
    float               m_min_x;
    float               m_max_y;
    float               m_min_y;
    float               m_max_x_backup;
    float               m_min_x_backup;
    float               m_max_y_backup;
    float               m_min_y_backup;
    float               m_max_x_best;
    float               m_min_x_best;
    float               m_max_y_best;
    float               m_min_y_best;
    int                 m_tf_seed;
           
    void m_initialize();
    void m_xy_reset();
    void m_xy_update(float s_xcoor, float l_xcoor, float s_ycoor, float l_ycoor);
    void m_TransformToBstarTree(vector<pair<float, float>> t_DieVec, 
                vector<vector<vector<TCG*>>>& t_ecgnode, 
                vector<pair<pair<float, float>, pair<float, float>>> t_CommonNetVec,
                vector<pair<int, int>> t_MappingCommonPinToDie);
};


class pair_comparator{
    public:
        bool operator()(pair<int, float> a, pair<int, float> b){
            return a.second > b.second;
        }
};

#endif