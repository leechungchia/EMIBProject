#ifndef TCG_H
#define TCG_H

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


using namespace std;

class EMIBP;
class EMIBNet;

class TCGNode{
    public:
        TCGNode(string t_code_name, float t_weight):
        m_code_name(t_code_name), m_initial_weight(t_weight), m_weight(t_weight), m_value(0), m_is_visited(1), m_depth(-1), m_visited_counter(0), m_rotated(0){}
        void   ValueGenerate();
        float  value(){return m_value;}
        void   Setvalue(float t_value){m_value = t_value;};
        float  weight(){return m_weight;}
        float  initial_weight(){return m_initial_weight;};
        void   SetDepth(int t_depth){m_depth = t_depth;}
        int    depth(){return m_depth;}
        bool   is_visited(){return (m_visited_counter == 0 && m_is_visited == 1);}
        void   SetVisited(queue<TCGNode*>& t_TCGNodeQueue);
        bool   finished(){return m_visited_counter == 0;};
        void   DecreaseCounter(){m_visited_counter -= 1;};
        void   ResetCounter(){m_visited_counter = m_BottomNodes.size();};
        void   EdgeConnect(set<TCGNode*>* t_InsertedNodes);
        void   UpperInsert(TCGNode* t_node){m_UpperNodes.insert(t_node);};
        void   BottomInsert(TCGNode* t_node){m_BottomNodes.insert(t_node);};
        string CodeName(){return m_code_name;};
        int    r(){return m_rotated;};
        void   set_r(){(m_rotated +1 == 4)?0:m_rotated+1;};
        void   HardInitialize();
        void   SoftInitialize();
        set<TCGNode*>* UpperNodes(){return &m_UpperNodes;};
        set<TCGNode*>* DirectUpperNodes(){return &m_DirectUpperNodes;};
        set<TCGNode*>* DirectBottomNodes(){return &m_DirectBottomNodes;};
        set<TCGNode*>* BottomNodes(){return &m_BottomNodes;};
        TCGNode*       DualNode(){return m_DualNode;};
        void           SetDualNode(TCGNode* t_node){m_DualNode = t_node;};
        float          Overlap(TCGNode* t_node);
        float          Distance(TCGNode* t_node);
        int            ID(){return m_ID;};
        int            EMIBCounter(){return m_EMIBCounter;};
        void           decreaseEMIBCounter(){m_EMIBCounter--;};
        bool           is_parsed(){return m_is_parsed;};
        void           setParsed(bool t_parsed){m_is_parsed = t_parsed;};
        vector<EMIBP*> EMIBPs;
        vector<EMIBNet*> EMIBs;
    private:        
        set<TCGNode*>    m_UpperNodes;
        set<TCGNode*>    m_DirectUpperNodes;
        set<TCGNode*>    m_BottomNodes;
        set<TCGNode*>    m_DirectBottomNodes;
        TCGNode*         m_BaseNode;
        TCGNode*         m_DualNode;
        float            m_weight;
        float            m_initial_weight;
        float            m_value;
        string           m_code_name;
        int              m_depth;
        int              m_visited_counter;
        bool             m_is_visited;
        int              m_rotated;
        int              m_ID;
        int              m_EMIBnum;
        int              m_EMIBCounter;
        bool             m_is_parsed;

        
        
        
        
        
        
        
};

class EMIBNet {
    public:
        EMIBNet(TCGNode* t_node_1, TCGNode* t_node_2, float t_overlap, float t_distance):m_node_1(t_node_1), m_node_2(t_node_2), m_distance(m_distance), m_overlap(t_overlap), m_is_visited(0){}
        bool isOverlapValid(bool t_state);
        bool isDistanceValid(bool t_state);
        bool Legalize(bool t_state);
        TCGNode* node1(){return m_node_1;};
        TCGNode* node2(){return m_node_2;};
        TCGNode* dualnode(TCGNode* t_node){return (t_node == m_node_1)?m_node_2:m_node_1;};
        bool  is_visited(){return m_is_visited;};
        void  setvisited(bool t_visit){m_is_visited = t_visit;};
    private:
        float    m_overlap;
        float    m_distance;
        TCGNode* m_node_1;
        TCGNode* m_node_2;
        bool     m_is_visited;
        
        
        
        
};

class EMIBP {
    public:
        EMIBP(vector<TCGNode*>& t_nodes, vector<vector<EMIBNet*>>& t_nets){
            for(int i=0; i<t_nodes.size(); ++i){
                m_nodes.push_back(t_nodes[i]);
                m_nets[t_nodes[i]] = t_nets[i];
            }
        }
        EMIBP(){}
        void Legalization(bool t_state, set<EMIBP*, EMIBP_comparator>& t_netvec);
        set<EMIBP*, EMIBP_comparator> upperEMIBP;
        set<EMIBP*, EMIBP_comparator> bottomEMIBP;
        int     index(){return m_index;};
    private:
        int  m_index;
        void m_getbottomEMIBP(); 
        vector<TCGNode*> m_nodes;
        map<TCGNode* , vector<EMIBNet*>> m_nets;
};

class CommonTCGPin: public pin{
    public:
        CommonTCGPin(string t_code_name, float t_width_diff, float t_height_diff):
        pin(t_code_name, t_width_diff, t_height_diff){}
        float              x(){
            return pin_xcoor(m_h_node->value(), m_h_node->initial_weight(), m_v_node->initial_weight(), m_h_node->r());
        }
        float              y(){
            return pin_ycoor(m_v_node->value(), m_h_node->initial_weight(), m_v_node->initial_weight(), m_h_node->r());
        }
        CommonTCGPin*           dual_pin(){return m_dual_pin;};
        TCGNode*                located_TCGNode(){return m_h_node;};
        void                    set_dual_pin(CommonTCGPin* t_dual_pin){m_dual_pin = t_dual_pin;};
        void                    set_TCGNode(TCGNode* t_h_node, TCGNode* t_v_node){m_h_node = t_h_node; m_v_node = t_v_node;};
    private:
        CommonTCGPin*         m_dual_pin;
        TCGNode*              m_h_node;
        TCGNode*              m_v_node;
        
        
        
};

struct uf_node{
    TCGNode* node;
    uf_node* uppernode;
}

class TCGGraph{
    public:
        TCGGraph(string t_direction_type):m_direction_type(t_direction_type){
            m_source = new TCGNode("source", 0);
            m_target = new TCGNode("target", 0);
        }
        void Initialize(vector<TCGNode*>* t_TCGNodes, bool t_is_activated);
        void Overlap_Legalization();
    private:
        void     m_CoorGenerate();
        bool     m_TraverseToBound(vector<TCGNode*>& t_bound, vector<EMIBP*>& t_EMIBPs);
        void     m_unionfind(vector<TCGNode*>& t_nodes, vector<vector<EMIBNet*>>& t_nets, vector<vector<int>>& t_record);
        uf_node* m_findroot(uf_node* t_node);
        EMIBP*   m_EMIBPSource;
        TCGNode* m_source;
        TCGNode* m_target;
        vector<vector<EMIBNet*>>* m_Nets;
        string   m_direction_type;
        vector<EMIBP*> m_EMIBPvec;
};

class TCG{
    public:
        TCG(){
            m_HCG = new TCGGraph("HCG");
            m_VCG = new TCGGraph("VCG");
        }
        void TCGConstruct(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap);
        void Initialize();
        vector<float> get_dies_coor(int t_die_index);

    private:
        TCGGraph* m_HCG;
        TCGGraph* m_VCG;
        vector<pair<TCGNode*, TCGNode*>> m_TCGNodes;
        vector<TCGNode*>                 m_HCGNodes;
        vector<TCGNode*>                 m_VCGNodes;
        vector<pair<CommonTCGPin*, CommonTCGPin*>> m_common_nets;
        unordered_map<int, TCGNode*> m_die_map;
        vector<vector<EMIBNet*>>     m_EMIBNets;
};



class node_comparator{
    public:
        bool operator()(TCGNode* a, TCGNode* b){return a->value() > b->value();};
};

class EMIBP_comparator{
    public:
        bool operator()(EMIBP* a, EMIBP* b){return a->index() < b->index()};
};

class uf_comparator{
    public: 
        bool operator()(uf_node* a, uf_node* b){return a->node < b->node; };
}
#endif


