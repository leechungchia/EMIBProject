#include "TCG.h"

void TCGNode::SetVisited(queue<TCGNode*>& t_TCGNodeQueue){
    m_is_visited = 1;
    for(int i=0; i<m_UpperNodes.size(); ++i){
        m_UpperNodes[i]->DecreaseCounter();
        if(m_UpperNodes[i]->finished()){
            t_TCGNodeQueue.push(m_UpperNodes[i]);
        }
    }
}

void TCGNode::ValueGenerate(){
    if(m_code_name != "source"){
        float current_max = FLT_MIN;
        float height      = 0;
        for(int i=0; i<m_BottomNodes.size(); ++i){
            height = m_BottomNodes[i]->value() + m_BottomNodes[i]->weight();
            if(height > current_max){
                current_max = height;
                m_BaseNode = m_BottomNodes[i];
            }
        }
        m_value = current_max;
    }
    else{
        m_value = 0;
    }
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

void TCGGraph::initialize(vector<TCGNode*>* t_TCGNodes){
}

