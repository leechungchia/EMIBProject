#ifndef BASE_H
#define BASE_H

#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>
#include "string"
#include "object.h"
#include "bstar.h"

using namespace std;


class die;
class CommonPin;


class die: public rectangle{
    public:
        die(string t_code_name, float t_width, float t_height, int t_ID):rectangle(t_code_name, t_width, t_height), m_ID(t_ID){}
        vector<CommonPin*>* CommonPinVector(){return &m_commonpin_vec;};
        int  ID(){return m_ID;};
        bool inECG(){return m_inECG;};
        void SetInECG(bool t_in){m_inECG = t_in;}; 
    private:
        vector<CommonPin*> m_commonpin_vec;
        int                m_ID;
        bool               m_inECG;
};


class CommonPin: public pin
{
public:
    CommonPin(string t_code_name, float t_width_diff, float t_height_diff):
    pin(t_code_name, t_width_diff, t_height_diff){}
    float              x(){
        return pin_xcoor(m_die->x(), m_die->w(), m_die->h(),m_die->r());
    }
    float              y(){
        return pin_ycoor(m_die->y(), m_die->w(), m_die->h(), m_die->r());
    }
    CommonPin*         dual_pin(){return m_dual_pin;};
    die*               located_die(){return m_die;};
    void               set_dual_pin(CommonPin* t_dual_pin){m_dual_pin = t_dual_pin;};
    void               set_die(die* t_die){
        m_die = t_die;
        m_die->CommonPinVector()->push_back(this);
    };
private:
    CommonPin*         m_dual_pin;
    die*               m_die;
};

class EMIB{
    public:
        EMIB(int t_die_1, int t_die_2, float t_overlap, float t_distance, float t_occupied):die_1(t_die_1), die_2(t_die_2), overlap(t_overlap), distance(t_distance), occupied(t_occupied){}  
        int die_1;
        int die_2;
        float overlap;
        float distance;
        float occupied;
};


class ECG{
    public:
        ECG(vector<TCGNode*> t_dieset, vector<EMIB*> t_EMIBset): dieset(t_dieset), EMIBset(t_EMIBset){}
        vector<pair<float, float>> TransformDieToTCG();
        vector<vector<float>>      TransformEMIBToTCG();
        vector<pair<int, int>>     MappingEMIBToDie();
        vector<TCGNode*>   dieset;
        vector<EMIB*>  EMIBset;
    private:
}













#endif