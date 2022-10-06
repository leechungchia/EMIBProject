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
#include <cstdlib>
#include "base.h"
#include "bstar.h"
#include "sa.h"
#include "TCG.h"


using namespace std;







class GlobalPlacer{
public:
    GlobalPlacer(char* die_input, char* EMIB_input, string net_input, int net_num, string mode, int t_seed, int t_initial_seed){
        m_seed = t_seed;
        m_initial_topology_seed = t_initial_seed;
        if(mode == "normal"){
            m_read_die_input(die_input);
            m_read_EMIB_input(EMIB_input);
            m_read_net_input(net_input, net_num);
        }
        else if(mode == "test"){
            m_random_die_generate(15, make_pair(6,12), make_pair(6,12));
            m_random_EMIB_generate(18, make_pair(1,2), make_pair(1,1), make_pair(1,1));
        }
        //cout << "Die Num: " << m_DieVec.size() << endl;
        //cout << "EMIB Num: " << m_EMIBNets.size() << endl;
        //cout << "Common Net Num: " << m_CommonNetVec.size() << endl;
        m_ECG_extraction(m_DieVec, m_EMIBNets, m_ECGs);
        for(auto it=m_ECGs.begin(); it!=m_ECGs.end();){
            if((*it)->dieset.size() == 1){
                m_NormalDieVec.push_back((*it)->dieset[0]);
                it = m_ECGs.erase(it);
            }
            else{
                ++it;
            }
        }
        for(int i=0; i<m_ECGs.size(); ++i){
            //cout << "ECG" << i << ":";
            for(int j=0; j<m_ECGs[i]->dieset.size(); ++j){
                //cout << m_ECGs[i]->dieset[j]->code_name() << ",";
                m_ECGs[i]->dieset[j]->set_initial_index2(j);
                m_inNormalDieVec.push_back(m_ECGs[i]->dieset[j]);
            }
            cout << endl;
            m_initial_topology_generation(m_ECGs[i]);
            //cout << "ECG " << i << " HCG edge num:" << m_ECGs[i]->h_edges.size() << endl;
            //cout << "ECG " << i << " VCG edge num:" << m_ECGs[i]->v_edges.size() << endl;
            //cout << endl;
            //cout << "ECG" << i << " EMIB num: " << m_ECGs[i]->EMIBset.size() << endl;
            SA* new_partialplacer = new SA();
            new_partialplacer->InputData(m_ECGs[i]->TransformDieToTCG(), m_ECGs[i]->TransformEMIBToTCG(), m_ECGs[i]->MappingEMIBToDie(), m_ECGs[i]->h_edges, m_ECGs[i]->v_edges, "TCG");
            PartialPlacers.push_back(new_partialplacer);
        }
        OverallPlacer = new SA();
        //cout << "Initialization Finished" << endl;
    }
    SA* OverallPlacer;
    vector<SA*> PartialPlacers;
    void write_output(char* arg1, char* arg2, char* arg3);
    void set_random_seed(int t_seed){m_seed = t_seed;};
    void set_initial_seed(int t_seed){m_initial_topology_seed = t_seed;};
    void do_partial_placement(){
        for(int i=0; i<m_ECGs.size(); ++i){
            vector<vector<float>> die_inf = PartialPlacers[i]->get_dies_inf(0);
            //cout << "updated die num: " << die_inf.size() << endl;
            for(int j=0; j<m_ECGs[i]->dieset.size(); ++j){
                m_ECGs[i]->dieset[j]->set_x(die_inf[j][0]);
                m_ECGs[i]->dieset[j]->set_y(die_inf[j][1]);
                m_ECGs[i]->dieset[j]->set_r(die_inf[j][2]);
                m_ECGs[i]->dieset[j]->set_initial_index(j);;
            }
        }
    }
    void do_overall_placement(){
        OverallPlacer->InputData(m_TransformDieToBstar(), m_TransformCommonNetToBstar(), m_MappingCommonPinToDie(), "B*-tree");
        OverallPlacer->start();
        vector<vector<float>> die_inf = OverallPlacer->get_dies_inf(0);
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
    void m_random_die_generate(int t_num, pair<int, int> t_width_range, pair<int, int> t_height_range);
    void m_random_EMIB_generate(int t_num, pair<int, int> t_overlap_range, pair<int, int> t_distance_range, pair<int, int> t_occupied_range);
    void m_tree_node_generation(vector<die*>& t_dies, vector<EMIB*>& t_EMIBs, vector<tree_net*>& t_treenets);
    void m_findroot(tree_node*& t_node, tree_node*& t_root);
    bool m_unioninsert(tree_net* t_net, vector<vector<die*>>& t_dietrees);
    void m_ECG_extraction(vector<die*>& t_dies, vector<EMIB*>& t_EMIBs, vector<ECG*>&  t_ECGs);
    void m_initial_topology_generation(ECG* t_ECGs);
    void m_branch_connect(vector<tree_node*>& t_trees);
    void m_maximum_spanning_tree(vector<die*>& t_dies, vector<EMIB*>& t_nets, MST_node*& t_root);
    void m_reduction_assignment(MST_node* t_root, vector<MST_node*> t_aboveroots,vector<vector<int>>& t_edges);
    void m_random_assignment(MST_node* t_root , vector<MST_node*>& t_bottom, vector<vector<int>>& t_edges);
    void m_graph_connection(MST_node* t_root1, MST_node* t_root2, vector<vector<int>>& t_h_edges, vector<vector<int>>& t_v_edges);
    int  m_search_die(string t_name);
    float m_check_similarity(TCG* t_tree1, TCG* t_tree2);
    vector<pair<float, float>> m_TransformDieToBstar();
    vector<pair<pair<float, float>, pair<float, float>>> m_TransformCommonNetToBstar();
    vector<pair<float, float>>  m_TransformECGToTCG();
    vector<pair<int, int>> m_MappingCommonPinToDie();
    vector<pair<int, int>> m_MappingEMIBToDie();
    vector<die*>  m_DieVec;
    vector<die*>  m_NormalDieVec;
    vector<die*> m_inNormalDieVec;
    vector<pair<CommonPin*, CommonPin*>> m_CommonNetVec;
    vector<EMIB*> m_EMIBNets;
    vector<ECG*>  m_ECGs;
    int           m_seed;
    int           m_initial_topology_seed;
};

#endif