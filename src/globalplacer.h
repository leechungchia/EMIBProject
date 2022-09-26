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
    GlobalPlacer(char* die_input, char* EMIB_input, string net_input, int net_num){
        m_seed = 30;
        m_read_die_input(die_input);
        m_read_EMIB_input(EMIB_input);
        m_read_net_input(net_input, net_num);
        cout << "Die Num: " << m_DieVec.size() << endl;
        cout << "EMIB Num: " << m_EMIBNets.size() << endl;
        cout << "Common Net Num: " << m_CommonNetVec.size() << endl;
        m_ECG_extraction();
        for(int i=0; i<m_ECGs.size(); ++i){
            cout << "ECG" << i << ":";
            for(int j=0; j<m_ECGs[i]->dieset.size(); ++j){
                cout << m_ECGs[i]->dieset[j]->code_name() << ",";
            }
            cout << endl;
            cout << "ECG" << i << " EMIB num: " << m_ECGs[i]->EMIBset.size() << endl;
            SA* new_partialplacer = new SA();
            new_partialplacer->InputData(m_ECGs[i]->TransformDieToTCG(), m_ECGs[i]->TransformEMIBToTCG(), m_ECGs[i]->MappingEMIBToDie(), "TCG");
            PartialPlacers.push_back(new_partialplacer);
        }
        OverallPlacer = new SA();
        cout << "Initialization Finished" << endl;
    }
    SA* OverallPlacer;
    vector<SA*> PartialPlacers;
    void write_output(char* arg1, char* arg2, char* arg3);
    void set_random_seed(int t_seed){m_seed = t_seed;}
    void do_partial_placement(){
        for(int i=0; i<m_ECGs.size(); ++i){
            vector<vector<float>> die_inf = PartialPlacers[i]->get_dies_inf();
            for(int j=0; j<m_ECGs[i]->dieset.size(); ++j){
                m_ECGs[i]->dieset[j]->set_x(die_inf[i][0]);
                m_ECGs[i]->dieset[j]->set_y(die_inf[i][1]);
                m_ECGs[i]->dieset[j]->set_r(die_inf[i][2]);
            }
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
    void m_read_EMIB_input(char* arg);
    void m_random_net_generate(int num);
    void m_ECG_extraction();
    int  m_search_die(string t_name);
    vector<pair<float, float>> m_TransformDieToBstar();
    vector<pair<pair<float, float>, pair<float, float>>> m_TransformCommonNetToBstar();
    vector<pair<float, float>>  m_TransformECGToTCG();
    vector<pair<int, int>> m_MappingCommonPinToDie();
    vector<pair<int, int>> m_MappingEMIBToDie();
    vector<die*>  m_DieVec;
    vector<pair<CommonPin*, CommonPin*>> m_CommonNetVec;
    vector<EMIB*> m_EMIBNets;
    vector<ECG*>  m_ECGs;
    int           m_seed;
};

#endif