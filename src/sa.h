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
    srand(m_seed);
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
            m_best_cost = get_current_cost();
            cout << "Construct B*-tree successfully" << endl;
        }
    }
    void InputData(vector<pair<float, float>> t_DieVec, 
                vector<vector<float>> t_EMIBNetVec, 
                vector<pair<int, int>> t_MappingEMIBToDie,
                string t_structure){
            cout << "TCG mode" << endl;
            m_TCG = new TCG();
            m_TCG->TCGConstruct(t_DieVec, t_EMIBNetVec, t_MappingEMIBToDie);
            cout << "TCGNode Constructed successfully" << endl;
            m_TCG->Initialize();
            cout << "TCG Initialization successfully" << endl;
    }
    void  set_random_seed(int t_seed);
    void  set_profile_parameter(float t_bound_arg, float t_area_arg, float t_hpwl_arg);
    void  set_SA_parameter(float t_initial_temperature, float t_decay_rate, float t_epsilon, unsigned  t_k, unsigned t_time_upperbound);
    void  set_boundary_parameter(float t_x_bound, float t_y_bound);
    void  set_bstar_parameter(float t_swap_p, float t_di_p, float t_rotate_p);
    float get_profile_cost();
    float get_area_cost();
    float get_hpwl_cost();
    float get_current_cost();
    float get_current_best_cost();
    void  start();
    vector<vector<float>> get_dies_inf();
private:
    //// data structure ////
    BstarTree*    m_BstarTree;
    TCG*          m_TCG;
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

    bool  m_chosen_probability(float diff, float current_temperature);
    void  m_bstar_move();
    void  m_bstar_start();
};

#endif