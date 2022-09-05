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
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>
#include "object.h"
#include "horizontal_contour.h"

using namespace std;

class BstarNode;
class CommonBstarPin;
class BstarTree;

class BstarNode: public rectangle
{
public:
    BstarNode(){}
    BstarNode(string t_code_name, float t_width, float t_height):
    rectangle(t_code_name, t_width, t_height), m_is_visited(false){
        m_backup = new BstarNode();
        m_best = new BstarNode();
    }
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
            return pin_xcoor(m_node->x(), m_node->w(), m_node->h(), m_node->r());
        }
        float              y(){
            return pin_ycoor(m_node->y(), m_node->w(), m_node->h(), m_node->r());
        }
        CommonBstarPin*         dual_pin(){return m_dual_pin;};
        BstarNode*              located_BstarNode(){return m_node;};
        void                    set_dual_pin(CommonBstarPin* t_dual_pin){m_dual_pin = t_dual_pin;};
        void                    set_BstarNode(BstarNode* t_node){m_node = t_node;};
    private:
        CommonBstarPin*         m_dual_pin;
        BstarNode*              m_node;
};

class BstarTree
{
public:
    BstarTree(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap)
    {
        m_TransformToBstarTree(t_NodeVec, t_PinVec, t_PinNodeMap);
        m_contour = new horizontal_contour;
        m_ground  = new BstarNode("ground",0,0);
        m_xy_reset();
        m_initialize();
        do_backups();
        restore_bests();
    }
    void swap_move(BstarNode* node_1, BstarNode* node_2);
    void rotate(BstarNode* t_node);
    void delete_insert(BstarNode* d_node, BstarNode* i_node, bool dim);
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
private:
    horizontal_contour* m_contour;
    BstarNode*               m_ground;
    BstarNode*               m_root;
    BstarNode*               m_root_backup;
    BstarNode*               m_root_best;
    vector<BstarNode*>  m_nodes;
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
    void m_initialize();
    void m_xy_reset();
    void m_xy_update(float s_xcoor, float l_xcoor, float s_ycoor, float l_ycoor);
    void m_TransformToBstarTree(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap);
};

#endif