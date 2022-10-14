#ifndef SA_H
#define SA_H

#include <map>
#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>
#include "bstar.h"
#include "TCG.h"
//#include "globalplacer.h"

using namespace std;

class GlobalPlacer;
class cost_comparator;
class SA
{
public:
    SA(){
    m_BstarTree = 0;
    m_structure = "None";
    //// input parameter ////
    m_size      = 0;
    //// profile parameter ////
    m_bound_arg = 0;
    m_area_arg  = 0.9;
    m_hpwl_arg  = 0.1;
    m_shape     = 0;
    m_x_bound   = FLT_MAX;
    m_y_bound   = FLT_MAX;
    //// SA parameter ////
    m_seed      = 0;
    m_initial_temperature = 0.1;
    m_decay_rate = 0.9;
    m_epsilon    = 0.0001;
    m_k          = 1000;
    m_time_upperbound = 50000;
    //// data record //// 
    m_best_area = 0;
    m_area_base = 0;
    m_hpwl_base = 0;
    m_best_cost = 0;
    //// movement probability ////
    m_swap_p    = 0.2;
    m_di_p      = 0.4;
    m_rotate_p  = 0.4;
    //// phase 1 parameter  ////
    m_initial_temperature_1 = 0.1;
    m_iternum   = 10000;
    m_similarity_bound    = 0.8;
    m_reduction_para      = 0.9;
    m_illegal_para        = 0.1;
    m_EMIB_overlap_para   = 0.01;
    m_buffer              = 100;
    m_group_num           = 0;
    m_topology_num        = 0;
    //// phase 2 parameter ////
    m_initial_temperature_2 = 0.1;
    m_iternum_2             = 10000;
    m_similarity_bound_2    = 0.9;
    m_buffer_2              = 100;
    }
    void InputData(vector<pair<float, float>> t_DieVec, 
                vector<pair<pair<float, float>, pair<float, float>>> t_CommonNetVec, 
                vector<pair<int, int>> t_MappingCommonPinToDie,
                string t_structure){
        cout << "Die InputSize: " << t_DieVec.size() << endl;
        cout << "Common Net InputSize: " << t_CommonNetVec.size() << endl;
        m_size = t_DieVec.size();
        m_time_upperbound = m_k*t_DieVec.size();
        m_structure = t_structure;
        if(m_structure == "B*-tree"){
            cout << "B* Tree Mode" << endl;
            m_BstarTree = new BstarTree(t_DieVec, t_CommonNetVec, t_MappingCommonPinToDie);
            m_best_area = m_BstarTree->area();
            m_area_base = m_BstarTree->area();
            m_hpwl_base = m_BstarTree->HPWL();
            //m_best_cost = get_current_cost();
            cout << "Construct B*-tree successfully" << endl;
        }
    }
    void InputData(vector<pair<float, float>> t_DieVec, 
                vector<vector<float>> t_EMIBNetVec, 
                vector<pair<int, int>> t_MappingEMIBToDie,
                vector<vector<int>> t_h_edges,
                vector<vector<int>> t_v_edges,
                string t_structure){
            cout << "TCG mode" << endl;
            TCG* new_TCG = new TCG();
            base_TCG = new_TCG;
            m_size = t_DieVec.size();
            m_time_upperbound = m_k*t_DieVec.size();
            m_structure = t_structure;
            base_TCG->TCGConstruct(t_DieVec, t_EMIBNetVec, t_MappingEMIBToDie);
            base_TCG->GetTCGEdge(t_h_edges, t_v_edges);
            //m_TCG->test_Legalization();
            cout << "TCGNode Constructed successfully" << endl;
    }
    void  set_random_seed(int t_seed);
    void  set_profile_parameter(float t_bound_arg, float t_area_arg, float t_hpwl_arg);
    void  set_SA_parameter(float t_initial_temperature, float t_decay_rate, float t_epsilon, unsigned  t_k, unsigned t_time_upperbound);
    void  set_boundary_parameter(float t_x_bound, float t_y_bound);
    void  set_bstar_parameter(float t_swap_p, float t_di_p, float t_rotate_p);
    float get_profile_cost();
    float get_area_cost();
    float get_hpwl_cost();
    float get_current_cost(int t_mode, vector<float> t_input);
    float phase1_cost();
    float get_current_best_cost();
    void  start();
    void  phase1_start();
    void  phase2_start();
    void  find_legal_solution(int t_num);
    void  set_placer(GlobalPlacer* t_placer, int t_index){placer = t_placer;ECG_index=t_index;};
    vector<vector<float>> get_dies_inf(int t_index_1, int t_index_2);
    vector<vector<TCG*>>  m_TCGs;
    TCG* base_TCG;
    vector<set<pair<TCG*, float>, cost_comparator>> solution_set;
    GlobalPlacer* placer;
    int           ECG_index;
private:
    //// data structure ////
    BstarTree*    m_BstarTree;
    vector<TCGNode*> m_legal_solutions;
    string        m_structure;
    //// input parameter ////
    int           m_size;
    float         m_space_width;
    float         m_space_height;
    //// profile parameter ////
    float         m_bound_arg;
    float         m_area_arg;
    float         m_hpwl_arg;
    float         m_shape;
    float         m_x_bound;
    float         m_y_bound;
    bool          m_is_bound_constraint;
    //// SA parameter ////
    int           m_seed;
    float         m_initial_temperature;
    float         m_decay_rate;
    float         m_epsilon;
    unsigned      m_k;
    unsigned      m_time_upperbound;
    //// data record //// 
    float         m_best_area;
    float         m_area_base;
    float         m_hpwl_base;
    float         m_best_cost;
    //// movement probability ////
    float         m_swap_p;
    float         m_di_p;
    float         m_rotate_p;
    //// phase 1 parameter ////
    float         m_initial_temperature_1;
    int           m_iternum;
    float         m_similarity_bound;
    float         m_reduction_para;
    float         m_illegal_para;
    float         m_EMIB_overlap_para;
    int           m_group_num;
    int           m_topology_num;
    int           m_buffer;
    //// phase 2 parameter ////
    float         m_initial_temperature_2;
    int           m_iternum_2;
    float         m_similarity_bound_2;
    int           m_buffer_2;
    float         m_x_para;
    float         m_y_para;
    float         m_delta_para;
    bool  m_chosen_probability(float diff, float current_temperature);
    void  m_bstar_move();
    void  m_bstar_start();
};


class cost_comparator{
    public:
        bool operator()(pair<TCGNode*, float> a, pair<TCGNode*, float> b){
            return a.second < b.second;
        }
};
#endif