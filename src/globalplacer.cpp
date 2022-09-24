#include "globalplacer.h"

void GlobalPlacer::m_read_die_input(char* arg)
{
	ifstream data(arg);
    if(!data)
    {
        cout << "die input error" << endl;
    }
    string name;
    float  w;
    float  h;
    while(data >> name)
    {
        data >> w;
        data >> h;
        die* temp = new die(name, w, h, m_DieVec.size());
        m_DieVec.push_back(temp);

    }
}

int GlobalPlacer::m_search_die(string t_name){
    for(int i=0; i<m_DieVec.size(); ++i){
        if(m_DieVec[i]->code_name() == t_name){
            return i;
        }
    }
    return -1;
}
void GlobalPlacer::m_read_EMIB_input(char* arg)
{
	ifstream data(arg);
    if(!data)
    {
        cout << "die input error" << endl;
    }
    string die_1;
    string die_2;
    float  overlap;
    float  distance;
    while(data >> die_1)
    {
        data >> die_2;
        data >> overlap;
        data >> distance;
        EMIB* temp = new EMIB(m_search_die(die_1), m_search_die(die_2), overlap, distance);
        m_EMIBNets.push_back(temp);
    }
}

void GlobalPlacer::m_read_net_input(string arg, int net_num)
{
    if(arg == "random")
    {
        cout << "random net generates." << endl;
        m_random_net_generate(net_num);
    }
}



void GlobalPlacer::m_random_net_generate(int num)
{
    srand(m_seed);
    int index_1;
    int index_2;
    die* bk_1;
    die* bk_2;
    float bk_1_w;
    float bk_1_h;
    float bk_2_w;
    float bk_2_h;
    CommonPin* pin_1;
    CommonPin* pin_2;
    for(int i=0; i<num; ++i)
    {
        index_1 = rand()%m_DieVec.size();
        index_2 = rand()%m_DieVec.size();
        while(index_2 == index_1)
        {
            index_2 = rand()%m_DieVec.size();
        }
        bk_1 = m_DieVec[index_1];
        bk_2 = m_DieVec[index_2];

        bk_1_w = (int)(bk_1->w()/(rand()%5+2));//rand()%(int)bk_1->width();
        bk_1_h = (int)(bk_1->h()/(rand()%5+2));//rand()%(int)bk_1->height();
        bk_2_w = (int)(bk_2->w()/(rand()%5+2));//rand()%(int)bk_2->width();
        bk_2_h = (int)(bk_2->h()/(rand()%5+2));//rand()%(int)bk_2->height();
        //bk_1_w = rand()%(int)bk_1->width();
        //bk_1_h = rand()%(int)bk_1->height();
        //bk_2_w = rand()%(int)bk_2->width();
        //bk_2_h = rand()%(int)bk_2->height();
        pin_1 = new CommonPin("CommonPin"+to_string(2*i), bk_1_w, bk_1_h);
        pin_2 = new CommonPin("CommonPin"+to_string(2*i+1), bk_2_w, bk_2_h);
        pin_1->set_dual_pin(pin_2);
        pin_2->set_dual_pin(pin_1);
        pin_1->set_die(bk_1);
        pin_2->set_die(bk_2);
        m_CommonNetVec.push_back(make_pair(pin_1, pin_2));
    }
}

void GlobalPlacer::write_output(char* arg1, char* arg2, char* arg3)
{
    ofstream bgraph;
    bgraph.open(arg1);
    die* current_die;
    pair<CommonPin*, CommonPin*> current_net;
    CommonPin* pin_1;
    CommonPin* pin_2;
    float width;
    float height;
    for(unsigned i=0; i<m_DieVec.size(); ++i)
    {
        current_die = m_DieVec.at(i);
        bgraph << i << " "<<current_die->x() << " "<< current_die->y() << " "<< current_die->current_w() <<
        " "<< current_die->current_h() << endl;
    }
    bgraph.close();

    ofstream bgraph_2;
    bgraph_2.open(arg2);
    for(int i=0; i<m_CommonNetVec.size(); ++i)
    {
        current_net = m_CommonNetVec.at(i);
        pin_1 = current_net.first;
        pin_2 = current_net.second;
        bgraph_2 << pin_1->x() << " " << pin_1->y() <<
        " " << pin_2->x() << " " << pin_2->y() << endl;
    }
    bgraph_2.close();
    //sa->write_output(arg1, arg3);
}

vector<pair<float, float>> GlobalPlacer::m_TransformDieToBstar(){
    vector<pair<float, float>> inf;
    for(int i=0; i<m_DieVec.size(); ++i){
        inf.push_back(make_pair(m_DieVec[i]->w(), m_DieVec[i]->h()));
    }
    return inf;
}
vector<pair<pair<float, float>, pair<float, float>>> GlobalPlacer::m_TransformCommonNetToBstar(){
    vector<pair<pair<float, float>, pair<float, float>>> inf;
    for(int i=0; i<m_CommonNetVec.size(); ++i){
        inf.push_back(make_pair(make_pair(m_CommonNetVec[i].first->w_diff(),m_CommonNetVec[i].first->h_diff()), make_pair(m_CommonNetVec[i].second->w_diff(), m_CommonNetVec[i].second->h_diff())));
    }
    return inf;
}

vector<pair<float, float>>  GlobalPlacer::m_TransformEMIBToTCG(){
    vector<pair<float, float>> inf;
    for(int i=0; i<m_EMIBNets.size(); ++i){
        inf.push_back(make_pair(m_EMIBNets[i]->overlap, m_EMIBNets[i]->distance));
    }
    return inf;
}
vector<pair<int, int>> GlobalPlacer::m_MappingCommonPinToDie(){
    vector<pair<int, int>> inf;
    for(int i=0; i<m_CommonNetVec.size(); ++i){
        inf.push_back(make_pair(m_CommonNetVec[i].first->located_die()->ID(), m_CommonNetVec[i].second->located_die()->ID()));
    }
    return inf;
}

vector<pair<int, int>> GlobalPlacer::m_MappingEMIBToDie(){
    vector<pair<float, float>> inf;
    for(int i=0; i<m_EMIBNets.size(); ++i){
        inf.push_back(make_pair(m_EMIBNets[i]->die_1, m_EMIBNets[i]->die_2));
    }
    return inf;
}


