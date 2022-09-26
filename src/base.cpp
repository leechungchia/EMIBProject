#include "base.h"
vector<pair<float, float>> ECG::TransformDieToTCG(){
    vector<pair<float, float>> inf;
    for(int i=0; i<dieset.size(); ++i){
        inf.push_back(make_pair(dieset[i]->w(), dieset[i]->h()));
    }
    return inf;
}

vector<vector<float>>  ECG::TransformEMIBToTCG(){
    vector<pair<float, float>> inf;
    for(int i=0; i<EMIBset.size(); ++i){
        vector<float> sub_inf;
        sub_inf.push_back(EMIBset[i]->overlap);
        sub_inf.push_back(EMIBset[i]->distance);
        sub_inf.push_back(EMIBset[i]->occupied);
        inf.push_back(sub_inf);
    }
    return inf;
}

vector<pair<int, int>> ECG::MappingEMIBToDie(){
    vector<pair<int, int>> inf;
    for(int i=0; i<EMIBset.size(); ++i){
        inf.push_back(make_pair(EMIBset[i]->die_1, EMIBset[i]->die_2));
    }
    return inf;
}

