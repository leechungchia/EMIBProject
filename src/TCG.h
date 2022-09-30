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
class TCGNode;
class EMIBP_comparator;
class Legalizer;


class TCGNode{
    public:
        TCGNode(string t_code_name, float t_weight):
        m_code_name(t_code_name), m_initial_weight(t_weight), m_weight(t_weight), m_value(-1), m_is_visited(1), m_depth(-1), m_visited_counter(0), m_rotated(0){}
        void   ValueGenerate();
        bool   ValueUpdate(queue<TCGNode*>& t_nodes);
        bool   ValueUpdate();
        float  value(){return m_value;}
        void   Setvalue(float t_value){m_value = t_value;};
        float  weight(){return m_weight;}
        float  initial_weight(){return m_initial_weight;};
        void   SetDepth(int t_depth){m_depth = t_depth;}
        int    depth(){return m_depth;}
        bool   finished(){return m_visited_counter == 0;};
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
        int            TraverseCounter(){return m_visited_counter;};
        void           ResetEMIBCounter(){m_EMIBCounter = EMIBs.size();};
        void           decreaseEMIBCounter(int t_num){m_EMIBCounter-=t_num;};
        void           DecreaseCounter(){m_visited_counter -= 1;};
        void           ResetCounter(){m_visited_counter = m_BottomNodes.size();};
        bool           is_visited(){return (m_visited_counter == 0 && m_is_visited == 1);}
        void           SetVisited();
        bool           is_parsed(){return m_is_parsed;};
        void           setParsed(bool t_parsed){m_is_parsed = t_parsed;};
        vector<EMIBP*> EMIBPs;
        vector<EMIBNet*> EMIBs;
    private:
        bool             m_above_exist(TCGNode* t_node); 
        string           m_code_name;
        /// TCG Inf ///        
        set<TCGNode*>    m_UpperNodes;
        set<TCGNode*>    m_DirectUpperNodes;
        set<TCGNode*>    m_BottomNodes;
        set<TCGNode*>    m_DirectBottomNodes;
        TCGNode*         m_BaseNode;
        TCGNode*         m_DualNode;
        int              m_ID;
        int              m_depth;
        /// Geometric Inf ///
        float            m_weight;
        float            m_initial_weight;
        float            m_value;
        int              m_rotated;
        /// EMIB Inf ///
        int              m_EMIBnum;
        /// Traversal Inf ///
        int              m_visited_counter;
        bool             m_is_visited;
        int              m_EMIBCounter;
        bool             m_is_parsed;   
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
        bool Legalization(vector<EMIBP*>& t_netset, map<EMIBNet*, int>& t_legal_check);
        void Legalization(vector<TCGNode*>& t_nodes);
        int     index(){return m_index;};
        void    setindex(int t_index){m_index = t_index;};
        map<TCGNode* , vector<EMIBNet*>>* NetMap(){return &m_nets;};
        void half_Initialize(){
            for(int j=0; j<m_nodes.size(); ++j){
                m_nodes[j]->EMIBPs.push_back(this);
                m_nodes[j]->decreaseEMIBCounter(m_nets[m_nodes[j]].size());
            } 
        }
    private:
        int  m_index;
        void m_getbottomEMIBP(); 
        vector<TCGNode*> m_nodes;
        map<TCGNode* , vector<EMIBNet*>> m_nets;
};

class EMIBP_comparator{
    public:
        bool operator()(EMIBP* a, EMIBP* b){return a->index() < b->index();};
};



//cdcdcdcdccd

class EMIBNet {
    public:
        EMIBNet(string t_code_name, TCGNode* t_node_1, TCGNode* t_node_2, float t_overlap, float t_distance, float t_occupied):m_code_name(t_code_name), m_node_1(t_node_1), m_node_2(t_node_2), m_distance(m_distance), m_overlap(t_overlap),m_occupied(t_occupied), m_is_visited(0){}
        bool isOverlapValid(float& t_overlap);
        bool isDistanceValid();
        bool Legalize();
        float OverlapValue(){return m_overlap;};
        TCGNode* node1(){return m_node_1;};
        TCGNode* node2(){return m_node_2;};
        TCGNode* dualnode(TCGNode* t_node){return (t_node == m_node_1)?m_node_2:m_node_1;};
        bool  is_visited(){return m_is_visited;};
        void  setvisited(bool t_visit){m_is_visited = t_visit;};
        void LowHighDerive(TCGNode*& t_high, TCGNode*& t_low){
            if(m_node_1->value() > m_node_2->value()){
                t_high = m_node_1;
                t_low = m_node_2;
            }
            else{
                t_high = m_node_2;
                t_low  = m_node_1;
            }
        };
    private:
        float    m_overlap;
        float    m_distance;
        float    m_occupied;
        TCGNode* m_node_1;
        TCGNode* m_node_2;
        bool     m_is_visited;
        string   m_code_name;      
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

class EMIBNet_comparator{
    public:
        bool operator()(EMIBNet* a, EMIBNet* b){
            float a_overlap; 
            float b_overlap;
            a->isOverlapValid(a_overlap);
            b->isOverlapValid(b_overlap);
            return a_overlap < b_overlap; 
            };
};

class node_comparator{
    public:
        bool operator()(TCGNode* a, TCGNode* b){return a->value() > b->value();};
};

class Legalizer{


    public:

        Legalizer(){};
        void FindIllegal(EMIBP* t_EMIBP, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet);
        void FindCritical(EMIBNet*& t_EMIBNet, TCGNode*& t_high, TCGNode*& t_low, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet);
        bool SetLegal(EMIBNet* t_EMIBNet);
        void CheckViolation(set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, set<EMIBNet*, EMIBNet_comparator>& t_newWrongSet);
        void UpdateAbove(TCGNode* t_node, set<TCGNode*>& t_NodeSet);
        void EMIBPUpdate(EMIBP* t_EMIBP, set<TCGNode*>& t_UpdateNodes);
        void SelfUpdate(TCGNode* t_node, set<TCGNode*>& t_UpdateNodeSet);
        bool BranchUpdate(EMIBNet* t_EMIBNet, TCGNode* t_high, TCGNode* t_low, set<EMIBNet*, EMIBNet_comparator>& t_newWrongSet, pair<TCGNode*, TCGNode*>& t_target, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, bool t_state);
        bool CompleteUpdate(EMIBNet* t_EMIBNet, TCGNode* t_high, TCGNode* t_low, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet);
        bool Legalization(EMIBP* t_EMIBP);
    private:

};

class uf_node{
    public:
        TCGNode* node;
        uf_node* uppernode;
};

class EMIBInf{
    public:
        EMIBInf(int t_node_1, int t_node_2, float t_overlap, float t_distance, float t_occupied):overlap(t_overlap), distance(t_distance), node_1(t_node_1), node_2(t_node_2), occupied(t_occupied){}
        float overlap;
        float distance;
        float occupied;
        int   node_1;
        int   node_2;
};

class TCGGraph{
    public:
        TCGGraph(string t_direction_type):m_direction_type(t_direction_type){
            m_source = new TCGNode("source", 0);
            m_target = new TCGNode("target", 0);
            m_EMIBdepth = 0;
            legal = new Legalizer;
        }
        void Initialize(vector<TCGNode*>* t_TCGNodes, bool t_is_activated);
        bool Overlap_Legalization();
        bool state(){return (m_direction_type == "VCG")?1:0;};
        void SetEMIBInf(vector<EMIBInf>* t_inf){m_EMIBInf = t_inf;};
        void EMIBNetDerive(vector<TCGNode*>* t_TCGNodes);
        void ConstraintEdgeAdd(vector<vector<int>>& t_edges);
        void OriginalTraverse();
        void DirectEdgeAdd();
        int EMIBnum(){
            int num = 0;
            for(int i=0; i<m_Nets.size(); ++i){
                num += m_Nets[i].size();
            }
            return num/2;
        }
        void set_TCGNodes(vector<TCGNode*>* t_TCG){m_TCGNodes = t_TCG;};
    private:
        void     m_CoorGenerate();
        bool     m_TraverseToBound(vector<TCGNode*>& t_bound, vector<EMIBP*>& t_EMIBPs);
        void     m_unionfind(vector<TCGNode*>& t_nodes, vector<vector<EMIBNet*>>& t_nets, vector<vector<int>>& t_record);
        uf_node* m_findroot(uf_node* t_node);
        void     m_UpdateAbove(vector<TCGNode*>& t_nodes);
        Legalizer* legal;
        EMIBP*   m_EMIBPSource;
        TCGNode* m_source;
        TCGNode* m_target;
        vector<TCGNode*>*        m_TCGNodes;
        vector<vector<EMIBNet*>> m_Nets;
        string   m_direction_type;
        vector<EMIBP*> m_EMIBPvec;
        int      m_EMIBdepth;
        vector<EMIBInf>* m_EMIBInf;
};

class TCG{
    public:
        TCG(){
            m_HCG = new TCGGraph("HCG");
            m_VCG = new TCGGraph("VCG");
        }
        void TCGConstruct(vector<pair<float, float>>& t_NodeVec, vector<vector<float>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap);
        void GetTCGEdge(vector<vector<int>> t_h_edges, vector<vector<int>> t_v_edges);
        void test_Legalization();
        vector<float> get_dies_coor(int t_die_index);

    private:
        TCGGraph* m_HCG;
        TCGGraph* m_VCG;
        vector<pair<TCGNode*, TCGNode*>> m_TCGNodes;
        vector<TCGNode*>                 m_HCGNodes;
        vector<TCGNode*>                 m_VCGNodes;
        vector<pair<CommonTCGPin*, CommonTCGPin*>> m_common_nets;
        unordered_map<int, TCGNode*> m_die_map;
        vector<EMIBInf>     m_EMIBInfs;
};

class uf_comparator{
    public:
        bool operator()(uf_node* a, uf_node* b){return a->node < b->node;}
};

#endif








 