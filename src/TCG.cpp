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

void EdgeConnect(vector<TCGNode*>& t_InsertedNodes){
    for(int i=0; i)
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

void TCGGraph::initialize(vector<TCGNode*>* t_TCGNodes, int t_max_height, vector<float>& t_distribution){
    vector<int> TCG_num;
    TCG_num.push_back(0);
    int counter = 0;
    int num;
    for(int i=0; i<t_distribution.size()-1; ++i){
        num = ((int)((float)t_TCGNodes.size()*t_distribution[i]) == 0)?1:(int)((float)t_TCGNodes.size()*t_distribution[i]);
        counter += num;
        TCG_num.push_back(counter);
    }
    TCG_num.push_back(TCG_num.size() - counter); 
    for(int i=0; i<t_max_height; ++i){
        for(int j=TCG_num[i]; j<TCG_num[i+1]; ++j){
            continue;
        }
    }    
}