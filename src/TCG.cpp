#include "TCG.h"

void TCGNode::SetVisited(queue<TCGNode*>& t_TCGNodeQueue){
    m_is_visited = 1;
    for(auto it=m_UpperNodes.begin(); it != m_UpperNodes.end(); ++it){
        (*it)->DecreaseCounter();
        if((*it)->finished()){
            t_TCGNodeQueue.push((*it));
        }
    }
}

void TCGNode::ValueGenerate(){
    if(m_code_name != "source"){
        float current_max = -55555555555;
        float height      = 0;
        for(auto it=m_BottomNodes.begin(); it != m_BottomNodes.end(); ++it){
            height = (*it)->value() + (*it)->weight();
            if(height > current_max){
                current_max = height;
                m_BaseNode = (*it);
            }
        }
        m_value = current_max;
    }
    else{
        m_value = 0;
    }
}

float TCGNode::Overlap(TCGNode* t_node){
    float value_1 = m_value;
    float max_1   = m_value+m_weight;
    float value_2 = t_node->value();
    float max_2   = t_node->weight();
    if(max_2 >= max_1){
        if(value_1 >= value_2){
            return m_weight;
        }
        else if(value_2 <= max_1){
            return max_1 - value_2;
        }
        else{
            return -(value_2 - max_1);
        }
    }
    else{
        if(value_2 >= value_1){
            return t_node->weight();
        }
        else if(value_1 <= max_2){
            return max_2 - value_1;
        }
        else{
            return -(value_1 - max_2);
        }
    }
}

float TCGNode::Distance(TCGNode* t_node){
    float value_1 = m_value;
    float max_1   = m_value+m_weight;
    float value_2 = t_node->value();
    float max_2   = t_node->weight();
    if(max_2 >= max_1){
        if(value_2 >= max_1){
            return value_2 - max_1;
        }
        else{
            return 0;
        }
    }
    else{
        if(value_1 >= max_2){
            return value_1 - max_2;
        }
        else{
            return 0;
        }
    }
}

bool EMIBNet::isOverlapValid(bool t_state){
    if(!t_state){
        return (m_node_1->Overlap(m_node_2) >= m_overlap);
    }
    else{
        return (m_node_1->DualNode()->Overlap(m_node_2->DualNode()) >= m_overlap);
    }
}

bool EMIBNet::isDistanceValid(bool t_state){
    if(!t_state){
        return (m_node_1->Distance(m_node_2) <= m_distance);
    }
    else{
        return (m_node_1->DualNode()->Distance(m_node_2->DualNode()) <= m_distance);
    }
}

void EMIBNet::Legalize(bool t_state){
    TCGNode* node_1;
    TCGNode* node_2;
    if(!t_state){
        node_1 = m_node_1;
        node_2 = m_node_2;
    }
    else{
        node_1 = m_node_1->DualNode();
        node_2 = m_node_2->DualNode();
    }
    if(node_1->value() > node_2->value()){
        node_2->Setvalue(node_2->value() + m_overlap - node_1->Overlap(node_2));
    }
    else{
        node_1->Setvalue(node_1->value() + m_overlap - node_1->Overlap(node_2));
    }
}

vector<EMIBP*> EMIBP::Legalization(){
    set<TCGNode*, node_comparator> node_set;
    vector<TCGNode*>               record;
    TCGNode*                       current_node;
    for(int i=0; i<m_nodes.size(); ++i){
        node_set.insert(m_nodes[i]);
    }
    while(node_set.size() != 0){
        current_node = node_set.begin();
        for(int i=0; i<m_nets[current_node].size(); ++i){
            
        }
    }


}
/*
vector<EMIBNet*> TCGGraph::m_TraverseToBound(vector<TCGNode*>& t_bound){
    queue<TCGNode*>            traverseQueue;
    vector<TCGNode*>           ToppestNodes;
    vector<vector<EMIBNet*>>   EMIBNets;
    TCGNode*                   current_node;
    for(int i=0; i<t_bound.size(); ++i){
        traverseQueue.push(t_bound[i]);
    }
    while(traverseQueue.size() != 0){
        current_node = traverseQueue.front();
        traverseQueue.pop();
        for(auto it = current_node->DirectUpperNodes()->begin(); it != current_node->DirectUpperNodes()->end(); ++it){
            if((*it)){}
        }
    }
}
*/
void TCGGraph::m_CoorGenerate(){
    queue<TCGNode*> NodeQueue;
    TCGNode* current_node;
    NodeQueue.push(m_source);
    current_node = m_source;
    while(current_node->CodeName() != "target"){
        current_node = NodeQueue.front();
        NodeQueue.pop();
        current_node->ValueGenerate();
        current_node->SetVisited(NodeQueue);
    }
    m_target->ValueGenerate();
}

void TCGGraph::Initialize(vector<TCGNode*>* t_TCGNodes, bool t_is_activated){
    m_source->BottomNodes()->clear();
    m_source->UpperNodes()->clear();
    m_target->BottomNodes()->clear();
    m_target->UpperNodes()->clear();
    for(int i=0; i<t_TCGNodes->size(); ++i){
        t_TCGNodes->at(i)->HardInitialize();
    }
    for(int i=0; i<t_TCGNodes->size(); ++i){
        t_TCGNodes->at(i)->UpperInsert(m_target);
        t_TCGNodes->at(i)->BottomInsert(m_source);
        m_source->UpperInsert(t_TCGNodes->at(i));
        m_target->BottomInsert(t_TCGNodes->at(i));
    }
    if(t_is_activated){
        for(int i=1; i<t_TCGNodes->size(); ++i){
            t_TCGNodes->at(i)->BottomInsert(t_TCGNodes->at(i-1));
            t_TCGNodes->at(i-1)->UpperInsert(t_TCGNodes->at(i));
        }
    }
    for(int i=0; i<t_TCGNodes->size(); ++i){
        t_TCGNodes->at(i)->ResetCounter();
    }
    m_source->ResetCounter();
    m_target->ResetCounter();
    m_CoorGenerate(); 
}

void TCGNode::HardInitialize(){
    m_UpperNodes.clear();
    m_BottomNodes.clear();
    m_weight = m_initial_weight;
    m_value = 0;
    m_BaseNode = 0;
    m_depth = -1;
    m_rotated = 0;
    SoftInitialize();
}

void TCGNode::SoftInitialize(){
    m_visited_counter = m_BottomNodes.size();
    m_is_visited = 0;
}

void TCG::TCGConstruct(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap){
    string node_name_h;
    string node_name_v;
    string pin_name_1;
    string pin_name_2;
    for(int i=0; i<t_NodeVec.size(); ++i){
        node_name_h = "TCGNodeH"+to_string(i);
        node_name_v = "TCGNodeV"+to_string(i);
        TCGNode* new_node_h = new TCGNode(node_name_h, t_NodeVec[i].first);
        TCGNode* new_node_v = new TCGNode(node_name_v, t_NodeVec[i].second);
        m_TCGNodes.push_back(make_pair(new_node_h, new_node_v));
        m_HCGNodes.push_back(new_node_h);
        m_VCGNodes.push_back(new_node_v);
        new_node_h->SetDualNode(new_node_v);
        new_node_v->SetDualNode(new_node_h);
        m_die_map[i] = new_node_h;
    }
    for(int i=0; i<t_PinVec.size(); ++i){
        pin_name_1 = "TCGPin_node" + to_string(t_PinNodeMap[i].first) + "_" + to_string(2*i);
        pin_name_1 = "TCGPin_node" + to_string(t_PinNodeMap[i].second) + "_" + to_string(2*i+1);
        CommonTCGPin* new_pin_1 = new CommonTCGPin(pin_name_1, t_PinVec[i].first.first, t_PinVec[i].first.second);
        CommonTCGPin* new_pin_2 = new CommonTCGPin(pin_name_2, t_PinVec[i].second.first, t_PinVec[i].second.second);
        new_pin_1->set_dual_pin(new_pin_2);
        new_pin_2->set_dual_pin(new_pin_1);
        new_pin_1->set_TCGNode(m_HCGNodes.at(t_PinNodeMap[i].first), m_VCGNodes.at(t_PinNodeMap[i].first));
        new_pin_2->set_TCGNode(m_HCGNodes.at(t_PinNodeMap[i].second), m_VCGNodes.at(t_PinNodeMap[i].second));
        m_common_nets.push_back(make_pair(new_pin_1, new_pin_2));
    }
}

void TCG::Initialize(){
    m_HCG->Initialize(&m_HCGNodes, 1);
    m_VCG->Initialize(&m_VCGNodes, 0);
}

vector<float> TCG::get_dies_coor(int t_die_index){
    vector<float> die_inf;
    TCGNode* target_node = m_die_map[t_die_index];
    die_inf.push_back(target_node->value());
    die_inf.push_back(target_node->DualNode()->value()); 
    die_inf.push_back(target_node->r());
    return die_inf;
}