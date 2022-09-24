#ifndef GLOBALPLACER_H
#define GLOBALPLACER_H

#include <map>
#include <queue>
#include <stack>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>
#include "base.h"
#include "bstar.h"
#include "sa.h"
#include "TCG.h"


using namespace std;

class GlobalPlacer{
public:
    GlobalPlacer(char* die_input, string net_input, int net_num){
        m_seed = 30;
        m_read_die_input(die_input);
        m_read_net_input(net_input, net_num);
        cout << "Die Num: " << m_DieVec.size() << endl;
        cout << "Common Net Num: " << m_CommonNetVec.size() << endl;
        OverallPlacer = new SA();
        SA* new_partialplacer = new SA();
        PartialPlacers.push_back(new_partialplacer);
        cout << "Initialization Finished" << endl;
    }
    GlobalPlacer(){
        die* t_0 = new die("t0", 3, 4, 0);
        die* t_1 = new die("t1", 2, 3, 1);
        die* t_2 = new die("t2", 3, 7, 2);
        die* t_3 = new die("t3", 1, 3, 3);
        die* t_4 = new die("t4", 1, 2, 4);
        m_DieVec.push_back(t_0);
        m_DieVec.push_back(t_1);
        m_DieVec.push_back(t_2);
        m_DieVec.push_back(t_3);
        m_DieVec.push_back(t_4);
        SA* new_partialplacer = new SA();
        PartialPlacers.push_back(new_partialplacer);
        cout << "Initialization Finished" << endl;
        PartialPlacers[0]->InputData(m_TransformDieToBstar(), m_TransformCommonNetToBstar(), m_MappingCommonPinToDie(), "TCG");
    }
    SA* OverallPlacer;
    vector<SA*> PartialPlacers;
    void write_output(char* arg1, char* arg2, char* arg3);
    void set_random_seed(int t_seed){m_seed = t_seed;}
    void do_partial_placement(){
        PartialPlacers[0]->InputData(m_TransformDieToBstar(), m_TransformCommonNetToBstar(), m_MappingCommonPinToDie(), "TCG");
        vector<vector<float>> die_inf = PartialPlacers[0]->get_dies_inf();
        for(int i=0; i<m_DieVec.size(); ++i){
            m_DieVec[i]->set_x(die_inf[i][0]);
            m_DieVec[i]->set_y(die_inf[i][1]);
            m_DieVec[i]->set_r(die_inf[i][2]);
        }
    }
    void do_overall_placement(){
        OverallPlacer->InputData(m_TransformDieToBstar(), m_TransformCommonNetToBstar(), m_MappingCommonPinToDie(), "B*-tree");
        OverallPlacer->start();
        vector<vector<float>> die_inf = OverallPlacer->get_dies_inf();
        for(int i=0; i<m_DieVec.size(); ++i){
            m_DieVec[i]->set_x(die_inf[i][0]);
            m_DieVec[i]->set_y(die_inf[i][1]);
            m_DieVec[i]->set_r(die_inf[i][2]);
        }
    };
    
private:
    void m_read_die_input(char* arg);
    void m_read_net_input(string arg, int net_num);
    void m_random_net_generate(int num);
    vector<pair<float, float>> m_TransformDieToBstar();
    vector<pair<pair<float, float>, pair<float, float>>> m_TransformCommonNetToBstar();
    vector<pair<int, int>> m_MappingCommonPinToDie();
    vector<die*> m_DieVec;
    vector<pair<CommonPin*, CommonPin*>> m_CommonNetVec;
    map<pair<die*, die*>, pair<float, float>> m_EMIBNet;
    int          m_seed;
};

#endif