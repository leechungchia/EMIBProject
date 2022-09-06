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
        float current_max = FLT_MIN;
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

void TCGNode::EdgeConnect(set<TCGNode*>* t_InsertedNodes){
    return;
}

void TCGGraph::m_CoorGenerate(){
    queue<TCGNode*> NodeQueue;
    TCGNode* current_node;
    NodeQueue.push(m_source);
    while(current_node->CodeName() != "target"){
        current_node = NodeQueue.front();
        NodeQueue.pop();
        current_node->ValueGenerate();
        current_node->SetVisited(NodeQueue);
    }
    m_target->ValueGenerate();
}

void TCGGraph::initialize(vector<TCGNode*>* t_TCGNodes, bool t_is_activated){
    m_source->UpperInsert(t_TCGNodes->at(0));
    t_TCGNodes->at(0)->BottomInsert(m_source);
    t_TCGNodes->at(0)->ResetCounter();
    for(int i=1; i<t_TCGNodes->size()-1; ++i){
        t_TCGNodes->at(i)->BottomInsert(t_TCGNodes->at(i-1));
        t_TCGNodes->at(i)->UpperInsert(t_TCGNodes->at(i+1));
        t_TCGNodes->at(i)->ResetCounter();
    }
    m_target->BottomInsert(t_TCGNodes->at(t_TCGNodes->size()-1));
    t_TCGNodes->at(t_TCGNodes->size()-1)->UpperInsert(m_target);
    t_TCGNodes->at(t_TCGNodes->size()-1)->ResetCounter();
    m_CoorGenerate();       
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