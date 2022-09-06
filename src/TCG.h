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

class TCGNode{
    public:
        TCGNode(string t_code_name, float t_weight):m_code_name(t_code_name), m_weight(t_weight), m_value(0), m_is_visited(1), m_depth(-1), m_visited_counter(0){}
        void   ValueGenerate();
        float  value(){return m_value;}
        float  weight(){return m_weight;}
        void   SetDepth(int t_depth){m_depth = t_depth;}
        int    depth(){return m_depth;}
        bool   is_visited(){return (m_visited_counter == 0 && m_is_visited == 1);}
        void   SetVisited(queue<TCGNode*>& t_TCGNodeQueue);
        bool   finished(){return m_visited_counter == 0;};
        void   DecreaseCounter(){m_visited_counter -= 1;};
        void   ResetCounter(){m_visited_counter = m_BottomNodes.size();};
        void   EdgeConnect(vector<TCGNode*>& t_InsertedNodes);
        string CodeName(){return m_code_name;};
        set<TCGNode*>* UpperNodes(){return &m_UpperNodes;};
        set<TCGNode*>* BottomNodes(){return &m_BottomNodes;}; 
    private:        
        set<TCGNode*> m_UpperNodes;
        set<TCGNode*> m_BottomNodes;
        TCGNode*         m_BaseNode;
        TCGNode*         m_DualNode;
        float            m_weight;
        float            m_value;
        string           m_code_name;
        int              m_depth;
        int              m_visited_counter;
        bool             m_is_visited;    
};


class TCGGraph{
    public:
        TCGGraph(string t_direction_type):m_direction_type(t_direction_type){
            m_source = new TCGNode("source", 0);
            m_target = new TCGNode("target", 0);
        }
        void initialize(vector<TCGNode*>* t_TCGNodes, int t_max_height, vector<float>& t_distribution);
    private:
        void     m_CoorGenerate();
        TCGNode* m_source;
        TCGNode* m_target;
        string   m_direction_type;
        
};

class TCG{
    public:
        TCG(){
            m_HCG = new TCGGraph("HCG");
            m_VCG = new TCGGraph("VCG");
        }
    private:
        TCGGraph* m_HCG;
        TCGGraph* m_VCG;
        vector<pair<TCGNode*, TCGNode*>> m_TCGNodes;
        vector<TCGNode*>                 m_HCGNodes;
        vector<TCGNode*>                 m_VCGNodes;
};
#endif
