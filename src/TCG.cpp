#include "TCG.h"
void TCGNode::SetVisited(){
    for(auto it=m_UpperNodes.begin(); it != m_UpperNodes.end(); ++it){
        (*it)->DecreaseCounter();
    }
}
void TCGNode::ValueGenerate(){
    if(m_code_name != "source"){
        float current_max = m_value;
        float height      = 0;
        for(auto it=DirectBottomNodes()->begin(); it != DirectBottomNodes()->end(); ++it){
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
bool TCGNode::ValueUpdate(queue<TCGNode*>& t_nodes){
    float height = 0;
    bool revised = 0;
    TCGNode* current_max = (*m_BottomNodes.begin());
    for(auto it=m_DirectBottomNodes.begin(); it != m_DirectBottomNodes.end(); ++it){
        height = (*it)->value() + (*it)->weight();
        if(height > m_value){
            m_value = height;
            m_BaseNode = (*it);
            revised = 1;
        }
        else{
            if(height > current_max->value()+current_max->weight()){
                m_BaseNode = (*it);
            }
        }
    }
    if(revised){
        for(auto it=m_DirectUpperNodes.begin(); it!=m_DirectUpperNodes.end(); ++it){
            t_nodes.push((*it));
        }
        return 1;
    }
    return 0;
}
bool TCGNode::ValueUpdate(){
    float height = 0;
    bool revised = 0;
    TCGNode* current_max = (*m_DirectBottomNodes.begin());
    for(auto it=m_DirectBottomNodes.begin(); it != m_DirectBottomNodes.end(); ++it){
        height = (*it)->value() + (*it)->weight();
        if(height > m_value){
            m_value = height;
            m_BaseNode = (*it);
            revised = 1;
        }
        else{
            if(height > current_max->value()+current_max->weight()){
                m_BaseNode = (*it);
                current_max = (*it);
            }
        }
    }
    return revised;
}
float TCGNode::Overlap(TCGNode* t_node){
    float value_1 = m_value;
    float max_1   = m_value+m_weight;
    float value_2 = t_node->value();
    float max_2   = value_2+t_node->weight();
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
void TCGNode::HardInitialize(){
    m_UpperNodes.clear();
    m_BottomNodes.clear();
    m_weight = m_initial_weight;
    m_value = -1;
    m_BaseNode = 0;
    m_depth = -1;
    m_rotated = 0;
    SoftInitialize();
}
void TCGNode::SoftInitialize(){
    ResetCounter();
    ResetEMIBCounter();
    m_is_parsed = 0;
    m_is_visited = 0;
    m_value = 0;
}
TCGNode* TCGNode::copyself(){
    TCGNode* copied = TCGNode(m_code_name, m_initial_weight);
    copied->set_r(m_rotated);
    copied->SetWeight(m_weight);
    copied->BottomNodes()->insert(m_BottomNodes.begin(), m_BottomNodes.end());
    copied->UpperNodes()->insert(m_UpperNodes.begin(), m_UpperNodes.end());
    copied->DirectBottomNodes()->insert(m_DirectBottomNodes.begin(), m_DirectBottomNodes.end());
    copied->DirectUpperNodes()->insert(m_DirectUpperNodes.begin(), m_DirectUpperNodes.end());
    copied->SetBackupValue(m_value);
    return copied;
}

float EMIBNet::distanceUpperBound(TCGNode* t_node){
    TCGNode* node_1 = (t_node == m_node_1)?(m_node_1):(m_node_2);
    TCGNode* node_2 = (t_node != m_node_1)?(m_node_1):(m_node_2);
    if(node_1->value() > node_2->value()){
        return m_distance - (node_1->value() - node_2->value() - node_2->weight());
    }
    return 0;
}
float EMIBNet::overlapUpperBound(TCGNode* t_node){
    TCGNode* node_1 = (t_node == m_node_1)?(m_node_1):(m_node_2);
    TCGNode* node_2 = (t_node != m_node_1)?(m_node_1):(m_node_2);
    float max_1 = node_1->value() + node_1->weight();
    float max_2 = node_2->value() + node_2->weight();
    return (max_2 - node_1->value() - m_overlap);

}
bool EMIBNet::isOverlapValid(float& t_overlap){
    t_overlap = m_node_1->Overlap(m_node_2);
    return (m_node_1->Overlap(m_node_2) >= m_overlap);
}
bool EMIBNet::isDistanceValid(){
    return (m_node_1->Distance(m_node_2) <= m_distance);
}
void TCGGraph::OriginalTraverse(){
    queue<TCGNode*>            traverseQueue;
    TCGNode*                   current_node;
    traverseQueue.push(m_source);
    for(int i=0; i<m_EMIBPvec.size(); ++i){
        delete m_EMIBPvec[i];
    }
    m_EMIBPvec.clear();
    for(auto it=m_TCGNodes->begin(); it!=m_TCGNodes->end(); ++it){
        (*it)->SoftInitialize();
    }
    m_source->SoftInitialize();
    m_target->SoftInitialize();
    while(traverseQueue.size() != 0){
        current_node = traverseQueue.front();
        traverseQueue.pop();
        current_node->SetVisited();
        for(auto it = current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
            if((*it)->finished()){
                (*it)->ValueGenerate();
                traverseQueue.push((*it));
            }
        }
    }
}
bool TCGGraph::Distance_Legalization(){
    set<TCGNode*, node_comparator> legal_order;
    for(int i=0; i<m_TCGNodes->size(); ++i){
        legal_order.insert(m_TCGNodes->at(i));
    }
    bool success;
    int counter = 0;
    for(auto it=legal_order.begin(); it!=legal_order.end(); ++it){
        for(int i=0; i< (*it)->DualNode()->EMIBs.size(); ++i){
            success = legal->DistanceLegalization((*it)->DualNode()->EMIBs[i]);
            if(!success){
                return 0;
            }
        }
    }
    return 1;
}
bool TCGGraph::m_TraverseToBound(vector<TCGNode*>& t_bound, vector<EMIBP*>& t_EMIBPs){
    queue<TCGNode*>            traverseQueue;
    vector<vector<EMIBNet*>>   EMIBNets;
    set<TCGNode*>           newToppestNodes;
    TCGNode*                   current_node;
    for(int i=0; i<t_bound.size(); ++i){
        traverseQueue.push(t_bound[i]);
    }
    while(traverseQueue.size() != 0){
        current_node = traverseQueue.front();
        traverseQueue.pop();
        if(current_node->EMIBCounter() > 0){
            newToppestNodes.insert(current_node);
        }
        else{
            current_node->SetVisited();
            for(auto it = current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                if((*it)->finished() && !(*it)->is_parsed()){
                    (*it)->ValueGenerate();
                    (*it)->setParsed(true);
                    traverseQueue.push((*it));
                }
            }
        }
    }
    for(auto it=newToppestNodes.begin(); it!=newToppestNodes.end(); ++it){
        vector<EMIBNet*> temp;
        for(int i=0; i<(*it)->EMIBs.size(); ++i){
            if(newToppestNodes.find((*it)->EMIBs[i]->dualnode((*it))) != newToppestNodes.end() && !(*it)->EMIBs[i]->finished){
                temp.push_back((*it)->EMIBs[i]);
                (*it)->EMIBs[i]->finished = true;
            }
        }
        EMIBNets.push_back(temp);
    }
    if(newToppestNodes.size() > 1 && EMIBNets[0].size() == 0){
        return 0;
    }
    vector<vector<int>> record;
    t_bound.clear();
    t_EMIBPs.clear();
    for(auto it=newToppestNodes.begin(); it!=newToppestNodes.end(); ++it){
        t_bound.push_back((*it));
    }
    if(EMIBNets.size() > 0){
        m_unionfind(t_bound, EMIBNets, record);
    }
    for(int i=0; i<record.size(); ++i){
        vector<TCGNode*> node_set;
        vector<vector<EMIBNet*>> net_set;
        for(int j=0; j<record[i].size(); ++j){
            node_set.push_back(t_bound[record[i][j]]);
            net_set.push_back(EMIBNets[record[i][j]]);
        }
        if(node_set.size() > 0){
            EMIBP* new_EMIBP = new EMIBP(node_set, net_set);
            new_EMIBP->setindex(m_EMIBdepth);
            m_EMIBPvec.push_back(new_EMIBP);
            t_EMIBPs.push_back(new_EMIBP);
        }
    }
    m_EMIBdepth++;
    return 1;
}
bool TCGGraph::Overlap_Legalization(){
    int counter = 0;
    vector<TCGNode*> bound;
    vector<EMIBP*>   bound_EMIBs;
    vector<EMIBP*> belowEMIBset;
    for(int i=0; i<m_EMIBPvec.size(); ++i){
        delete m_EMIBPvec[i];
    }
    m_EMIBPvec.clear();
    for(auto it=m_TCGNodes->begin(); it!=m_TCGNodes->end(); ++it){
        (*it)->SoftInitialize();
    }
    m_source->SoftInitialize();
    m_target->SoftInitialize();
    bound.push_back(m_source);
    bool graph_state = state();
    bool           bound_success;
    bool           legalization_success;
    //cout << "Toppest bound stage" << endl;
    bound_success = m_TraverseToBound(bound, bound_EMIBs);
    while(bound_success && bound_EMIBs.size() > 0){
        for(int i=0; i<bound_EMIBs.size(); ++i){
            //cout << "Legalization stage" << endl;
            legalization_success = legal->Legalization(bound_EMIBs[i]);
            if(!legalization_success){
                if(counter - bound_EMIBs[i]->m_nodes.size() > 0)?(counter - bound_EMIBs[i]->m_nodes.size()):(0);
                m_valid_ratio = counter/m_TCGNodes->size();
                return 0;
            }

            bound_EMIBs[i]->half_Initialize();
        }
        //cout << "Toppest bound stage" << endl;
        counter += bound.size();
        bound_success = m_TraverseToBound(bound, bound_EMIBs);
    }
    if(!bound_success){
        cout << "no connection violation" << endl;
        m_EMIBdepth = 0;
        m_valid_ratio = 0;
        return 0;
    }
    float overlap;
    /*for(int i=0; i<m_Nets.size(); ++i){
        for(int j=0; j<m_Nets[i].size(); ++j){
            cout << m_Nets[i][j]->isOverlapValid(overlap) << endl;
            cout << m_Nets[i][j]->node1()->CodeName() << ":::" << m_Nets[i][j]->node2()->CodeName() << endl; 
        }
    }*/
    m_EMIBdepth = 0;
    m_valid_ratio = counter/m_TCGNodes->size();
    return 1;
}
uf_node* TCGGraph::m_findroot(uf_node* t_node){
    uf_node* temp = t_node;
    vector<uf_node*> revised_nodes;
    while(temp->uppernode !=0){
        revised_nodes.push_back(temp);
        temp = temp->uppernode;
    }
    for(int i=0; i<revised_nodes.size(); ++i){
        revised_nodes[i]->uppernode = temp;
    }
    return temp;
}
void  TCGGraph::m_unionfind(vector<TCGNode*>& t_nodes, vector<vector<EMIBNet*>>& t_nets, vector<vector<int>>& t_record){
    map<TCGNode*, int> node_map;
    for(int i=0; i<t_nodes.size(); ++i){
        node_map[t_nodes[i]] = i;
    }
    uf_node* root = new uf_node;
    TCGNode* root_node = (*t_nodes.begin());
    set<TCGNode*> exist_set;
    set<uf_node*, uf_comparator> uf_subnodes;
    uf_node temp;
    uf_node temp_2;
    set<uf_node*, uf_comparator>::iterator temp_iter;
    set<uf_node*, uf_comparator>::iterator temp_iter_2;
    uf_node* temp_node;
    uf_node* temp_root;
    uf_node* temp_2_root;
    bool is_root;
    bool temp_exist;
    bool temp_2_exist;
    root->node = root_node;
    root->uppernode = 0;
    uf_subnodes.insert(root);
    for(int i=0; i<t_nets[0].size(); ++i){
        uf_node* sub_node = new uf_node;
        sub_node->node = (t_nets[0][i]->dualnode(root_node));
        sub_node->uppernode = root;
        uf_subnodes.insert(sub_node);
    }
    for(int i=1; i<t_nodes.size(); ++i){
        is_root = 0;
        temp.node = t_nodes[i];
        temp_iter = uf_subnodes.find(&temp);
        temp_exist = !(temp_iter == uf_subnodes.end());
        temp_node = (temp_exist)?(*temp_iter):(0);
        for(int j=0; j<t_nets[i].size(); ++j){
            temp_2.node = t_nets[i][j]->dualnode(t_nodes[i]);
            temp_iter_2 = uf_subnodes.find(&temp_2);
            temp_2_exist = !(temp_iter_2 == uf_subnodes.end());
            if(!temp_exist && !temp_2_exist){
                uf_node* new_root = new uf_node;
                new_root->node = temp.node;
                new_root->uppernode = 0;
                uf_node* new_node = new uf_node;
                new_node->node = temp_2.node;
                new_node->uppernode = new_root;
                uf_subnodes.insert(new_root);
                uf_subnodes.insert(new_node);
                temp_exist = 1;
                temp_node = new_root;
            }
            else if(!temp_exist && temp_2_exist){
                uf_node* new_node = new uf_node;
                new_node->node = temp.node;
                new_node->uppernode = (*temp_iter_2);
                temp_exist = 1;
                uf_subnodes.insert(new_node);
                temp_node = new_node;
            }
            else if(temp_exist && !temp_2_exist){
                uf_node* new_node = new uf_node;
                new_node->node = temp_2.node;
                new_node->uppernode = temp_node;
                uf_subnodes.insert(new_node); 
            }
            else{
                temp_root = m_findroot(temp_node);
                temp_2_root = m_findroot((*temp_iter_2));
                if(temp_root != temp_2_root){
                    temp_2_root->uppernode = temp_root;
                }
            }
        }
    }
    map<TCGNode*, int> set_map;
    TCGNode*           root_TCG;
    for(auto it=uf_subnodes.begin(); it!=uf_subnodes.end(); ++it){
        root_TCG = m_findroot((*it))->node;
        if(set_map.find(root_TCG) == set_map.end()){
            set_map[root_TCG] = set_map.size();
            vector<int> sub_record;
            sub_record.push_back(node_map[(*it)->node]);
            t_record.push_back(sub_record);
        }
        else{
            t_record[set_map[root_TCG]].push_back(node_map[(*it)->node]);
        }
    }
    for(auto it=uf_subnodes.begin(); it!=uf_subnodes.end(); ++it){
        delete (*it);
    }
}
void TCGGraph::EMIBNetDerive(vector<TCGNode*>* t_TCGNodes){
    m_Nets.clear();
    TCGNode* node_1;
    TCGNode* node_2;
    string code_name;
    float overlap;
    float distance;
    float occupied;
    for(int i=0; i<t_TCGNodes->size(); ++i){
        vector<EMIBNet*> temp;
        m_Nets.push_back(temp);
        for(int j=0; j<t_TCGNodes->at(i)->EMIBs.size(); ++j){
            delete t_TCGNodes->at(i)->EMIBs[j];
        }
        t_TCGNodes->at(i)->EMIBs.clear();
        t_TCGNodes->at(i)->EMIBPs.clear();
    }
    for(int i=0; i<m_EMIBInf->size(); ++i){
        node_1 = t_TCGNodes->at(m_EMIBInf->at(i).node_1);
        node_2 = t_TCGNodes->at(m_EMIBInf->at(i).node_2);
        if(node_1->UpperNodes()->find(node_2) == node_1->UpperNodes()->end() && node_2->UpperNodes()->find(node_1) == node_2->UpperNodes()->end()){
            code_name = to_string(m_EMIBInf->at(i).node_1) + "_" + to_string(m_EMIBInf->at(i).node_2);
            overlap = m_EMIBInf->at(i).overlap;
            distance = m_EMIBInf->at(i).distance;
            occupied = m_EMIBInf->at(i).occupied;
            EMIBNet* net = new EMIBNet(code_name+"_1", node_1, node_2, overlap, distance, occupied);
            EMIBNet* net_2 = new EMIBNet(code_name+"_2", node_2, node_1, overlap, distance, occupied);
            m_Nets[m_EMIBInf->at(i).node_1].push_back(net);
            node_1->EMIBs.push_back(net);
            m_Nets[m_EMIBInf->at(i).node_2].push_back(net_2);
            node_2->EMIBs.push_back(net_2);
        }
    }
}
void TCGGraph::ConstraintEdgeAdd(vector<vector<int>>& t_edges){
    cout << "TCG edge num: " << t_edges.size() << endl;
    for(int i=0; i<t_edges.size(); ++i){
        m_TCGNodes->at(t_edges[i][0])->UpperInsert(m_TCGNodes->at(t_edges[i][1]));
        m_TCGNodes->at(t_edges[i][1])->BottomInsert(m_TCGNodes->at(t_edges[i][0]));
    }
    for(int i=0; i<m_TCGNodes->size(); ++i){
        m_source->UpperInsert(m_TCGNodes->at(i));
        m_target->BottomInsert(m_TCGNodes->at(i));
        m_TCGNodes->at(i)->BottomInsert(m_source);
        m_TCGNodes->at(i)->UpperInsert(m_target);
    }
    for(int i=0; i<m_TCGNodes->size(); ++i){
        m_TCGNodes->at(i)->ResetCounter();
    }
    m_source->ResetCounter();
    m_target->ResetCounter();
}
void TCGGraph::DirectEdgeAdd(){
    bool above_exist = true;
    TCGNode* current_above_node;
    TCGNode* current_node;
    m_source->DirectUpperNodes()->clear();
    m_target->DirectBottomNodes()->clear();
    for(auto it=m_TCGNodes->begin(); it!=m_TCGNodes->end(); ++it){
        (*it)->DirectUpperNodes()->clear();
        (*it)->DirectBottomNodes()->clear();
    }
    for(int i=0; i<m_TCGNodes->size(); ++i){
        current_node = m_TCGNodes->at(i);
        for(auto it=current_node->UpperNodes()->begin(); it!=current_node->UpperNodes()->end(); ++it){
            current_above_node = (*it);
            above_exist = true;
            for(auto it_2=current_node->UpperNodes()->begin(); it_2!=current_node->UpperNodes()->end(); ++it_2){
                if(((*it)!=(*it_2)) && ( (*it_2)->UpperNodes()->find((*it)) != (*it_2)->UpperNodes()->end())){
                    above_exist = false;
                    break;
                }
            }
            if(above_exist){
                current_node->DirectUpperNodes()->insert(current_above_node);
                current_above_node->DirectBottomNodes()->insert(current_node);
            }
        }
    }
    for(auto it=m_source->UpperNodes()->begin(); it!=m_source->UpperNodes()->end(); ++it){
        current_above_node = (*it);
        above_exist = true;
        for(auto it_2=m_source->UpperNodes()->begin(); it_2!=m_source->UpperNodes()->end(); ++it_2){
            if((it!=it_2) && ( (*it_2)->UpperNodes()->find((*it)) != (*it_2)->UpperNodes()->end())){
                above_exist = false;
                break;
            }
        }
        if(above_exist){
            m_source->DirectUpperNodes()->insert(current_above_node);
            current_above_node->DirectBottomNodes()->insert(m_source);
        }
    }
}
void TCGGraph::RemoveEdge(TCGNode* t_below, TCGNode* t_upper){
    t_below->UpperNodes()->erase(t_upper);
    //t_below->DirectUpperNodes()->erase(t_upper);
    t_upper->BottomNodes()->erase(t_below);
    //t_upper->DirectBottomNodes()->erase(t_below);
}
void TCGGraph::AddEdge(TCGNode* t_below, TCGNode* t_upper){
    t_below->UpperNodes()->insert(t_upper);
    t_upper->BottomNodes()->insert(t_below);
}






void Legalizer::FindIllegal(EMIBP* t_EMIBP, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet){
    t_WrongSet.clear();
    t_CorrectSet.clear();
    float overlap;
    for(auto it=t_EMIBP->NetMap()->begin(); it!=t_EMIBP->NetMap()->end(); ++it){
        for(int i=0; i<(it->second).size();++i){
            if(!(it->second)[i]->isOverlapValid(overlap)){
                t_WrongSet.insert((it->second)[i]);
            }
            else{
                t_CorrectSet.insert((it->second)[i]);
            }
        }
    }
}
void Legalizer::FindCritical(EMIBNet*& t_EMIBNet, TCGNode*& t_high, TCGNode*& t_low, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet){
    EMIBNet* current_net = (*t_WrongSet.begin());
    current_net->LowHighDerive(t_high, t_low);
    t_EMIBNet = current_net;
}
bool Legalizer::SetLegal(EMIBNet* t_EMIBNet){
    TCGNode* node_1;
    TCGNode* node_2;
    node_1 = t_EMIBNet->node1();
    node_2 = t_EMIBNet->node2();
    float overlap = t_EMIBNet->OverlapValue();
    if(overlap > node_1->weight() && overlap > node_2->weight()){
        return 0;
    }
    if(node_1->value() > node_2->value()){
        node_2->Setvalue(node_2->value() + overlap - node_1->Overlap(node_2));
    }
    else{
        node_1->Setvalue(node_1->value() + overlap - node_1->Overlap(node_2));
    }
    return 1;
}
void Legalizer::CheckViolation(set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, set<EMIBNet*, EMIBNet_comparator>& t_newWrongSet){
    float new_overlap;
    for(auto it=t_CorrectSet.begin(); it!=t_CorrectSet.end();){
        if(!(*it)->isOverlapValid(new_overlap)){
            t_newWrongSet.insert((*it));
            t_CorrectSet.erase(it++);
        }
        else{
            ++it;
        }
    }
}
void Legalizer::UpdateAbove(TCGNode* t_node, set<TCGNode*>& t_NodeSet){
    bool revised=false;
    for(auto it=t_node->DirectUpperNodes()->begin(); it!=t_node->DirectUpperNodes()->end(); ++it){
        revised = false;
        if((*it)->is_parsed()){
            revised = (*it)->ValueUpdate();
            if(revised){
                //cout << "above" << (*it)->CodeName() << endl;
                t_NodeSet.insert((*it));
            }
        }
    }
}
void Legalizer::EMIBPUpdate(EMIBP* t_EMIBP, set<TCGNode*>& t_UpdateNodes){
    set<EMIBNet*, EMIBNet_comparator> WrongSet;
    set<EMIBNet*, EMIBNet_comparator> new_WrongSet;
    set<EMIBNet*, EMIBNet_comparator> new_WrongSet_2;
    set<EMIBNet*, EMIBNet_comparator> CorrectSet;
    FindIllegal(t_EMIBP, WrongSet, CorrectSet);
    TCGNode* high;
    TCGNode* low;
    EMIBNet* net;
    while(WrongSet.size() > 0){
        FindCritical(net, high, low, WrongSet);
        SetLegal(net);
        CorrectSet.insert(net);
        t_UpdateNodes.insert(low);
        CheckViolation(CorrectSet, new_WrongSet);
        while(new_WrongSet.size() > 0){
            (*new_WrongSet.begin())->LowHighDerive(high, low);
            SetLegal((*new_WrongSet.begin()));
            CorrectSet.insert((*new_WrongSet.begin()));
            //cout << "pull up::" << low->CodeName() << endl;
            t_UpdateNodes.insert(low);
            CheckViolation(CorrectSet, new_WrongSet_2);
            new_WrongSet.erase(new_WrongSet.begin());
            for(auto it=new_WrongSet_2.begin(); it!=new_WrongSet_2.end(); ++it){
                new_WrongSet.insert((*it));
            }
            new_WrongSet_2.clear();
        }
        CorrectSet.clear();
        WrongSet.clear();
        FindIllegal(t_EMIBP, WrongSet, CorrectSet);
    }
}
void Legalizer::SelfUpdate(TCGNode* t_node, set<TCGNode*>& t_UpdateNodeSet, bool t_top){
    set<EMIBP*, EMIBP_comparator> EMIBPSet;
    EMIBPSet.insert(t_node->EMIBPs.begin(), t_node->EMIBPs.end());
    if(!t_top){
        UpdateAbove(t_node, t_UpdateNodeSet);
    }
    for(auto it=EMIBPSet.begin(); it!=EMIBPSet.end(); ++it){
        //cout << "Legaliztion stage:::::::Complete stage::::::Branch stage::::::self stage::::::EMIBUpdate" << endl;
        EMIBPUpdate((*it), t_UpdateNodeSet);
    }
    //cout << "ddddd "<< t_node->CodeName() << endl;
    for(auto it=t_UpdateNodeSet.begin(); it!=t_UpdateNodeSet.end(); ++it){
        //cout << "innnnnn  "<< (*it)->CodeName() << endl;
    }
}
bool Legalizer::BranchUpdate(EMIBNet* t_EMIBNet, TCGNode* t_high, TCGNode* t_low, set<EMIBNet*, EMIBNet_comparator>& t_newWrongSet, pair<TCGNode*, TCGNode*>& t_target, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, bool t_state){
    set<TCGNode*> UpdateNodes;
    set<TCGNode*> new_UpdateNodes;;
    bool overlap_success = SetLegal(t_EMIBNet);
    if(!overlap_success){
        cout << "weird" << endl;
        return 0;
    }
    //cout << "Legaliztion stage:::::::Complete stage::::::Branch stage::::::self stage 1" << endl;
    int min = 0;
    SelfUpdate(t_low, UpdateNodes, 1);
    set<TCGNode*, node_comparator> ProcessOrder;
    TCGNode*      current_node;
    TCGNode*      high;
    TCGNode*      low;
    float         overlap;
    for(auto it=UpdateNodes.begin(); it!=UpdateNodes.end(); ++it){
        ProcessOrder.insert((*it));
    }
    while(ProcessOrder.size() > 0){
        //cout << "ccccccc" << t_high->CodeName()<< "mmmmmm" << t_low->CodeName() << endl;
        //cout << "high:::::" << t_high->CodeName() << "," << "low::::::" << t_low->CodeName() << endl;
        current_node = (*ProcessOrder.begin());
        //cout << "Legaliztion stage:::::::Complete stage::::::Branch stage::::::self stage 0" << endl;
        SelfUpdate(current_node, new_UpdateNodes, 0);
        /*for(auto it=new_UpdateNodes.begin(); it!=new_UpdateNodes.end(); ++it){
            cout << (*it)->CodeName() << "kkkkkkkkkkkk" << (*it)->value()<<"jj";
        }
        cout << endl;*/
        ProcessOrder.erase(ProcessOrder.begin());
        for(auto it=new_UpdateNodes.begin(); it!=new_UpdateNodes.end(); ++it){
            ProcessOrder.insert((*it));
        }
        /*for(auto it=ProcessOrder.begin(); it!=ProcessOrder.end(); ++it){
            cout << (*it)->CodeName() << ",";
        }
        cout << endl;*/
        new_UpdateNodes.clear();
    }
    if(!t_EMIBNet->isOverlapValid(overlap)){
        cout << "unlegalizable branch: " << t_EMIBNet->node1()->CodeName() << ","  << t_EMIBNet->node2()->CodeName() << endl;
        return 0;
    }
    else{
        CheckViolation(t_CorrectSet, t_newWrongSet);
        if(t_state){
            t_target.first = t_high;
            t_target.second = t_low;
        }
        else{
            for(auto it=t_newWrongSet.begin(); it!=t_newWrongSet.end(); ++it){
                (*it)->LowHighDerive(high, low);
                if(high == t_target.first && low == t_target.second || (high == t_target.second && low == t_target.first)){
                    cout << "unlegalizable EMIBP: " << t_EMIBNet->node1()->CodeName() << ","  << t_EMIBNet->node2()->CodeName() << endl;
                    return 0;
                }
            }
        }
        t_CorrectSet.insert(t_EMIBNet);
        return 1;
    }
}
bool Legalizer::CompleteUpdate(EMIBNet* t_EMIBNet, TCGNode* t_high, TCGNode* t_low, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, set<EMIBNet*, EMIBNet_comparator>& t_WrongSet){
    set<EMIBNet*, EMIBNet_comparator> newWrongSet;
    set<EMIBNet*, EMIBNet_comparator> newWrongSet_2;
    pair<TCGNode*, TCGNode*> target;
    TCGNode* high;
    TCGNode* low;
    EMIBNet* current_net;
    //cout << "Legaliztion stage:::::::Complete stage::::::Branch stage" << endl;
    bool success = BranchUpdate(t_EMIBNet, t_high, t_low, newWrongSet, target, t_CorrectSet, 1);
    if(!success){
        return 0;
    }
    while(newWrongSet.size() > 0){
        current_net = (*newWrongSet.begin());
        current_net->LowHighDerive(high, low);
        //cout << "Legaliztion stage:::::::Complete stage::::::Branch stage" << endl;
        success = BranchUpdate(current_net, high, low, newWrongSet_2, target, t_CorrectSet, 0);
        if(!success){
            return 0;
        }
        newWrongSet.erase(newWrongSet.begin());
        newWrongSet.insert(newWrongSet_2.begin(), newWrongSet_2.end());
        newWrongSet_2.clear();
    }
    return 1;
}
bool Legalizer::Legalization(EMIBP* t_EMIBP){
    set<EMIBNet*, EMIBNet_comparator> CorrectSet;
    set<EMIBNet*, EMIBNet_comparator> WrongSet;
    TCGNode* high;
    TCGNode* low;
    EMIBNet* current_net;
    bool success = false;
    int iteration = 0;
    FindIllegal(t_EMIBP, WrongSet, CorrectSet);
    while(WrongSet.size() > 0){
        FindCritical(current_net, high, low, WrongSet);
        int CorrectSet_size = CorrectSet.size();
        //cout << "Legaliztion stage:::::::Complete stage" << endl;
        success = CompleteUpdate(current_net, high, low, CorrectSet, WrongSet);
        if(!success){
            return 0;
        }
        FindIllegal(t_EMIBP, WrongSet, CorrectSet);
        iteration++;
    }
    return 1;
}
bool Legalizer::DistanceLegalization(EMIBNet* t_EMIBNet){
    TCGNode* high;
    TCGNode* low;
    TCGNode* temp;
    t_EMIBNet->LowHighDerive(high, low);
    if(high->DualNode()->value() < low->DualNode()->value()){
        temp = high;
        high = low;
        low = temp;
    }
    float max_legal_length;
    bool have_value=false;
    float expected_length = high->DualNode()->value() - low->DualNode()->value() - low->DualNode()->weight() - t_EMIBNet->DistanceValue();
    if(expected_length <= 0){
        return 1;
    }
    for(auto it=low->DualNode()->EMIBs.begin(); it!=low->DualNode()->EMIBs.end(); ++it){
        have_value = true;
        if(it == low->DualNode()->EMIBs.begin()){
            max_legal_length = (*it)->overlapUpperBound(low->DualNode());
        }
        else{
            if(max_legal_length > (*it)->overlapUpperBound(low->DualNode())){
                max_legal_length = (*it)->overlapUpperBound(low->DualNode());
            }
        }
    }
    for(auto it=low->EMIBs.begin(); it!=low->EMIBs.end(); ++it){
        if(!have_value){
            max_legal_length = (*it)->distanceUpperBound(low);
        }
        else{
            if(max_legal_length > (*it)->distanceUpperBound(low)){
                max_legal_length = (*it)->distanceUpperBound(low);
            }
        }
    }
    
    if(expected_length <= max_legal_length){
        low->Setvalue(low->value() + expected_length);
        return 1;
    }
    else{
        return 0;
    }
}
bool Legalizer::EMIBOverlapCheck(){
    return
}



TCG* TCG::copyself(){
    TCG* copied;
    TCGNode* copied_h;
    TCGNode* copied_v;
    for(int i=0; i<m_TCGNodes.size(); ++i){
        copied_h = m_TCGNodes[i].first->copyself();
        copied_v = m_TCGNodes[i].second->copyself();
        copied_h->SetDualNode(copied_v);
        copied_v->SetDualNode(copied_h);
        copied->m_TCGNodes.push_back(make_pair(copied_h, copied_v));
        copied->m_HCGNodes.push_back(copied_h);
        copied->m_VCGNodes.push_back(copied_v);
    }
    for(int j=0; j<m_EMIBInfs.size(); ++j){
        copied->m_EMIBInfs.push_back(m_EMIBInfs[j]);
    }
    return copied;

}
void TCG::GetTCGEdge(vector<vector<int>> t_h_edges, vector<vector<int>> t_v_edges){
    m_HCG->ConstraintEdgeAdd(t_h_edges);
    m_HCG->DirectEdgeAdd();
    m_VCG->ConstraintEdgeAdd(t_v_edges);
    m_VCG->DirectEdgeAdd();
    m_HCG->EMIBNetDerive(&m_HCGNodes);
    m_VCG->EMIBNetDerive(&m_VCGNodes);
    for(int i=0; i<m_HCGNodes.size(); ++i){
        m_HCGNodes[i]->SoftInitialize();
    }
    for(int i=0; i<m_VCGNodes.size(); ++i){
        m_VCGNodes[i]->SoftInitialize();
    }

    //cout << "HCG EMIB num: "<< m_HCG->EMIBnum() << endl;
    //cout << "VCG EMIB num: "<< m_VCG->EMIBnum() << endl;
    //cout << "Classify EMIB net successfully" << endl;
}
void TCG::TCGConstruct(vector<pair<float, float>>& t_NodeVec, vector<vector<float>>& t_EMIBVec, vector<pair<int, int>>& t_PinNodeMap){
    string node_name_h;
    string node_name_v;
    string pin_name;
    for(int i=0; i<t_NodeVec.size(); ++i){
        node_name_h = "TCGNodeH"+to_string(i);
        node_name_v = "TCGNodeV"+to_string(i);
        TCGNode* new_node_h = new TCGNode(node_name_h, t_NodeVec[i].first);
        TCGNode* new_node_v = new TCGNode(node_name_v, t_NodeVec[i].second);
        new_node_h->setInitialindex(i);
        new_node_v->setInitialindex(i);
        m_TCGNodes.push_back(make_pair(new_node_h, new_node_v));
        m_HCGNodes.push_back(new_node_h);
        m_VCGNodes.push_back(new_node_v);
        new_node_h->SetDualNode(new_node_v);
        new_node_v->SetDualNode(new_node_h);
        m_die_map[i] = new_node_h;
    }
    for(int i=0; i<t_EMIBVec.size(); ++i){
        float overlap = t_EMIBVec[i][0];
        float distance = t_EMIBVec[i][1];
        float occupied = t_EMIBVec[i][2];
        int   node_1 = t_PinNodeMap[i].first;
        int   node_2 = t_PinNodeMap[i].second;
        pin_name = "TCGEMIB_node" + to_string(t_PinNodeMap[i].first) + "_" + to_string(t_PinNodeMap[i].first);
        EMIBInf inf(node_1, node_2, overlap, distance, occupied);
        m_EMIBInfs.push_back(inf);
    }
    for(int i=0; i<m_EMIBInfs.size(); ++i){
        cout << "EMIB " << i << ": " << m_EMIBInfs[i].node_1 <<"," << m_EMIBInfs[i].node_2<< endl;
    }
    cout << "TCG total EMIB num: "  << m_EMIBInfs.size() << endl;
    m_HCG->set_TCGNodes(&m_HCGNodes);
    m_VCG->set_TCGNodes(&m_VCGNodes);
    m_HCG->SetEMIBInf(&m_EMIBInfs);
    m_VCG->SetEMIBInf(&m_EMIBInfs);
    for(int i=0; i<m_HCGNodes.size(); ++i){
        m_HCGNodes[i]->HardInitialize();
    }
    for(int i=0; i<m_VCGNodes.size(); ++i){
        m_VCGNodes[i]->HardInitialize();
    }
    //cout << "Read TCG EMIB Inf" << endl;
}
void TCG::test_Legalization(){
    //movement_swap(6,7);
    int die1;
    int die2;
    int move;
    bool h_overlap_success;
    bool v_overlap_success;
    bool h_distance_success=true;
    bool v_distance_success=true;
    string h_success;
    string v_success;
    h_overlap_success = m_HCG->Overlap_Legalization();
    v_overlap_success = m_VCG->Overlap_Legalization();
    h_success = (h_overlap_success)?("success"):("false");
    v_success = (v_overlap_success)?("success"):("false");
    cout << "iteration "<< i <<" HCG Overlap Legalization: " << h_success << endl;
    cout << "iteration "<< i <<" VCG Overlap Legalization: " << v_success << endl;
    if(h_overlap_success && v_overlap_success){
        h_distance_success = m_HCG->Distance_Legalization();
        v_distance_success = m_VCG->Distance_Legalization();
        h_success = (h_distance_success)?("success"):("false");
        v_success = (v_distance_success)?("success"):("false");

        cout << "iteration " << i <<" HCG Distance Legalization: " << h_success << endl;
        cout << "iteration " << i <<" VCG Distance Legalization: " << v_success << endl;
        if(h_distance_success && v_distance_success){
            m_success = true;
            break;
        }
    }  
    
    
    //m_HCG->OriginalTraverse();
    //m_VCG->OriginalTraverse();
    
    
}
vector<float> TCG::get_dies_coor(int t_die_index){

    vector<float> die_inf;
    TCGNode* target_node = m_die_map[t_die_index];
    die_inf.push_back(target_node->value());
    die_inf.push_back(target_node->DualNode()->value()); 
    die_inf.push_back(target_node->r());
    return die_inf;
}
void TCG::movement_rotation(int t_die){
    m_TCGNodes[t_die].first->set_r();
    for(int i=0; i<m_HCG->m_Nets.size(); ++i){
        for(int j=0; j<m_HCG->m_Nets[i].size(); ++j){
            m_HCG->m_Nets[i][j]->finished = false;
        }
    }
    for(int i=0; i<m_VCG->m_Nets.size(); ++i){
        for(int j=0; j<m_VCG->m_Nets[i].size(); ++j){
            m_VCG->m_Nets[i][j]->finished = false;
        }
    }
    for(int i=0;i<m_HCG->m_TCGNodes->size(); ++i){
        m_HCG->m_TCGNodes->at(i)->EMIBPs.clear();
    }
    for(int i=0;i<m_VCG->m_TCGNodes->size(); ++i){
        m_VCG->m_TCGNodes->at(i)->EMIBPs.clear();
    }
}
void TCG::movement_swap(int t_die1, int t_die2){
    for(int i=0; i<m_EMIBInfs.size();++i){
        if(m_EMIBInfs[i].node_1 == t_die1){
            m_EMIBInfs[i].node_1 = t_die2;
        }
        else if(m_EMIBInfs[i].node_1 == t_die2){
            m_EMIBInfs[i].node_1 = t_die1;
        }
        if(m_EMIBInfs[i].node_2 == t_die1){
            m_EMIBInfs[i].node_2 = t_die2;
        }
        else if(m_EMIBInfs[i].node_2 == t_die2){
            m_EMIBInfs[i].node_2 = t_die1;
        }
    }
    TCGNode* die1_h=m_TCGNodes[t_die1].first;
    TCGNode* die2_h=m_TCGNodes[t_die2].first;
    TCGNode* die1_v=m_TCGNodes[t_die1].second;
    TCGNode* die2_v=m_TCGNodes[t_die2].second;
    float temp_weight1_h=die1_h->weight();
    float temp_initial_weight1_h=die1_h->initial_weight();
    float temp_weight1_v=die1_v->weight();
    float temp_initial_weight1_v=die1_v->initial_weight();
    string temp_codename1_h = die1_h->CodeName();
    string temp_codename1_v = die1_v->CodeName();
    int temp_initial_index1_h = die1_h->Initialindex();
    int temp_initial_index1_v = die1_v->Initialindex();
    int temp_r1_h = die1_h->r();
    int temp_r1_v = die1_v->r();
    m_die_map[m_TCGNodes[t_die1].first->Initialindex()] = die2_h;
    m_die_map[m_TCGNodes[t_die2].first->Initialindex()] = die1_h;
    die1_h->SetWeight(die2_h->weight());
    die1_h->SetInitialWeight(die2_h->initial_weight());
    die1_h->SetCodeName(die2_h->CodeName());
    die1_h->set_r(die2_h->r());
    die1_h->setInitialindex(die2_h->Initialindex());
    die1_v->SetWeight(die2_v->weight());
    die1_v->SetInitialWeight(die2_v->initial_weight());
    die1_v->SetCodeName(die2_v->CodeName());
    die1_v->set_r(die2_v->r());
    die1_v->setInitialindex(die2_v->Initialindex());

    die2_h->SetWeight(temp_weight1_h);
    die2_h->SetInitialWeight(temp_initial_weight1_h);
    die2_h->SetCodeName(temp_codename1_h);
    die2_h->set_r(temp_r1_h);
    die2_h->setInitialindex(temp_initial_index1_h);
    die2_v->SetWeight(temp_weight1_v);
    die2_v->SetInitialWeight(temp_initial_weight1_v);
    die2_v->SetCodeName(temp_codename1_v);
    die2_v->set_r(temp_r1_v);
    die2_v->setInitialindex(temp_initial_index1_v);
    m_HCG->EMIBNetDerive(&m_HCGNodes);
    m_VCG->EMIBNetDerive(&m_VCGNodes);
}
void TCG::movement_reverse(int t_die){
    /*for(int i=0;i<m_HCGNodes.size(); ++i){
        cout << m_HCGNodes[i]->CodeName() << endl;
        for(auto it=m_HCGNodes[i]->DirectBottomNodes()->begin(); it!=m_HCGNodes[i]->DirectBottomNodes()->end(); ++it){
            cout << (*it)->CodeName() << ",";
        }
        cout << endl;
    }
    for(int i=0;i<m_VCGNodes.size(); ++i){
        cout << m_VCGNodes[i]->CodeName() << endl;
        for(auto it=m_VCGNodes[i]->DirectBottomNodes()->begin(); it!=m_VCGNodes[i]->DirectBottomNodes()->end(); ++it){
            cout << (*it)->CodeName() << ",";
        }
        cout << endl;
    }
    cout << "lll cd" << endl;*/
    int in_h = rand()%2;
    int is_up = rand()%2;
    int target_index;
    int counter = 0;
    TCGNode* current_node;
    TCGNode* target;
    current_node = (in_h == 0)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
    while(counter < 2){
        if(is_up == 0 && current_node->UpperNodes()->size() > 1){
            target_index = rand()%(current_node->DirectUpperNodes()->size());
            for(auto it=current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                if(target_index == 0){
                    target = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
            break;
        }
        else if(is_up == 1 && current_node->BottomNodes()->size() > 1){
            target_index = rand()%(current_node->DirectBottomNodes()->size());
            target = current_node;
            for(auto it=current_node->DirectBottomNodes()->begin(); it!=current_node->DirectBottomNodes()->end(); ++it){
                if(target_index == 0){
                    current_node = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
            break;
        }
        else if(is_up == 0 && current_node->UpperNodes()->size() == 1){
            if(current_node->BottomNodes()->size() > 1){
                target_index = rand()%(current_node->DirectBottomNodes()->size());
            for(auto it=current_node->DirectBottomNodes()->begin(); it!=current_node->DirectBottomNodes()->end(); ++it){
                if(target_index == 0){
                    target = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
                break;
            }
            else{
                current_node=(in_h == 1)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
                in_h = (in_h == 0)?(1):(0);
                counter++;
            }
        }
        else if(is_up == 1 && current_node->BottomNodes()->size() == 1){
            if(current_node->UpperNodes()->size() > 1){
                target_index = rand()%(current_node->DirectUpperNodes()->size());
                target =current_node;
                for(auto it=current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                    if(target_index == 0){
                        current_node = (*it);
                        break;
                    }
                    else{
                        target_index--;
                    }
                }
                break;
            }
            else{
                current_node=(in_h == 1)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
                in_h = (in_h == 0)?(1):(0);
                counter++;
            }
        }
    }
    if(counter == 2){
        return;
    }
    vector<TCGNode*> Fin_target;
    vector<TCGNode*> Fout_current;
    for(auto it=current_node->UpperNodes()->begin(); it!=current_node->UpperNodes()->end(); ++it){
        if((*it) != target){
            Fout_current.push_back((*it));
        }
    }
    for(auto it=target->BottomNodes()->begin(); it!=target->BottomNodes()->end(); ++it){
        if((*it) != current_node){
            Fin_target.push_back((*it));
        }
    }
    //cout << current_node->CodeName() << "--->" << target->CodeName() << endl;
    Fout_current.push_back(current_node);
    Fin_target.push_back(target);
    if(in_h==0){
        m_HCG->RemoveEdge(current_node, target);
    }
    else{
        m_VCG->RemoveEdge(current_node, target);
    }
    
    for(auto it=Fin_target.begin(); it!=Fin_target.end(); ++it){
        for(auto it_2=Fout_current.begin(); it_2!=Fout_current.end(); ++it_2){
            if((*it)->UpperNodes()->find((*it_2)) == (*it)->UpperNodes()->end()){
                if(in_h == 0){
                    m_HCG->AddEdge((*it), (*it_2));
                    if((*it)->DualNode()->UpperNodes()->find((*it_2)->DualNode()) == (*it)->DualNode()->UpperNodes()->end()){
                        m_VCG->RemoveEdge((*it_2)->DualNode(), (*it)->DualNode());
                    }
                    else{
                        m_VCG->RemoveEdge((*it)->DualNode(), (*it_2)->DualNode());
                    }
                }
                else{
                    m_VCG->AddEdge((*it), (*it_2));
                    if((*it)->DualNode()->UpperNodes()->find((*it_2)->DualNode()) == (*it)->DualNode()->UpperNodes()->end()){
                        m_HCG->RemoveEdge((*it_2)->DualNode(), (*it)->DualNode());
                    }
                    else{
                        m_HCG->RemoveEdge((*it)->DualNode(), (*it_2)->DualNode());
                    }
                }
            }
        }
    }
    m_HCG->DirectEdgeAdd();
    m_VCG->DirectEdgeAdd();
    /*
    for(int i=0;i<m_HCGNodes.size(); ++i){
        cout << m_HCGNodes[i]->CodeName() << endl;
        for(auto it=m_HCGNodes[i]->DirectBottomNodes()->begin(); it!=m_HCGNodes[i]->DirectBottomNodes()->end(); ++it){
            cout << (*it)->CodeName() << ",";
        }
        cout << endl;
    }
    for(int i=0;i<m_VCGNodes.size(); ++i){
        cout << m_VCGNodes[i]->CodeName() << endl;
        for(auto it=m_VCGNodes[i]->DirectBottomNodes()->begin(); it!=m_VCGNodes[i]->DirectBottomNodes()->end(); ++it){
            cout << (*it)->CodeName() << ",";
        }
        cout << endl;
    }*/
    m_HCG->EMIBNetDerive(&m_HCGNodes);
    m_VCG->EMIBNetDerive(&m_VCGNodes);
}
void TCG::movement_move(int t_die){

    int in_h = rand()%2;
    int is_up = rand()%2;
    int target_index;
    int counter = 0;
    TCGNode* current_node;
    TCGNode* target;
    current_node = (in_h == 0)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
    while(counter < 2){
        if(is_up == 0 && current_node->UpperNodes()->size() > 1){
            target_index = rand()%(current_node->DirectUpperNodes()->size());
            for(auto it=current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                if(target_index == 0){
                    target = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
            break;
        }
        else if(is_up == 1 && current_node->BottomNodes()->size() > 1){
            target_index = rand()%(current_node->DirectBottomNodes()->size());
            target = current_node;
            for(auto it=current_node->DirectBottomNodes()->begin(); it!=current_node->DirectBottomNodes()->end(); ++it){
                if(target_index == 0){
                    current_node = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
            break;
        }
        else if(is_up == 0 && current_node->UpperNodes()->size() == 1){
            if(current_node->BottomNodes()->size() > 1){
                target_index = rand()%(current_node->DirectBottomNodes()->size());
            for(auto it=current_node->DirectBottomNodes()->begin(); it!=current_node->DirectBottomNodes()->end(); ++it){
                if(target_index == 0){
                    target = (*it);
                    break;
                }
                else{
                    target_index--;
                }
            }
                break;
            }
            else{
                current_node=(in_h == 1)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
                in_h = (in_h == 0)?(1):(0);
                counter++;
            }
        }
        else if(is_up == 1 && current_node->BottomNodes()->size() == 1){
            if(current_node->UpperNodes()->size() > 1){
                target_index = rand()%(current_node->DirectUpperNodes()->size());
                target =current_node;
                for(auto it=current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                    if(target_index == 0){
                        current_node = (*it);
                        break;
                    }
                    else{
                        target_index--;
                    }
                }
                break;
            }
            else{
                current_node=(in_h == 1)?(m_TCGNodes[t_die].first):(m_TCGNodes[t_die].second);
                in_h = (in_h == 0)?(1):(0);
                counter++;
            }
        }
    }
    if(counter == 2){
        return;
    }
    vector<TCGNode*> Fin_current;
    vector<TCGNode*> Fout_target;
    for(auto it=current_node->DualNode()->BottomNodes()->begin(); it!=current_node->DualNode()->BottomNodes()->end(); ++it){
        Fin_current.push_back((*it));
    }
    for(auto it=target->DualNode()->UpperNodes()->begin(); it!=target->DualNode()->UpperNodes()->end(); ++it){
        Fout_target.push_back((*it));
    }
    Fin_current.push_back(current_node->DualNode());
    Fout_target.push_back(target->DualNode());
    if(in_h==0){
        m_HCG->RemoveEdge(current_node, target);
    }
    else{
        m_VCG->RemoveEdge(current_node, target);
    }
    
    for(auto it=Fin_current.begin(); it!=Fin_current.end(); ++it){
        for(auto it_2=Fout_target.begin(); it_2!=Fout_target.end(); ++it_2){
            if((*it)->UpperNodes()->find((*it_2)) == (*it)->UpperNodes()->end()){
                if(in_h == 0){
                    m_VCG->AddEdge((*it), (*it_2));
                    if((*it)->DualNode()->UpperNodes()->find((*it_2)->DualNode()) == (*it)->DualNode()->UpperNodes()->end()){
                        m_HCG->RemoveEdge((*it_2)->DualNode(), (*it)->DualNode());
                    }
                    else{
                        m_HCG->RemoveEdge((*it)->DualNode(), (*it_2)->DualNode());
                    }
                }
                else{
                    m_HCG->AddEdge((*it), (*it_2));
                    if((*it)->DualNode()->UpperNodes()->find((*it_2)->DualNode()) == (*it)->DualNode()->UpperNodes()->end()){
                        m_VCG->RemoveEdge((*it_2)->DualNode(), (*it)->DualNode());
                    }
                    else{
                        m_VCG->RemoveEdge((*it)->DualNode(), (*it_2)->DualNode());
                    }
                }
            }
        }
    }
    m_HCG->DirectEdgeAdd();
    m_VCG->DirectEdgeAdd();
    m_HCG->EMIBNetDerive(&m_HCGNodes);
    m_VCG->EMIBNetDerive(&m_VCGNodes);
}
void TCG::construct_similar_map(){
    for(int i=0; i<m_TCGNodes.size(); ++i){
        similar_map_h[m_TCGNodes[i].first] = m_TCGNodes[i].first->m_BaseNode;
        similar_map_v[m_TCGNodes[i].first] = m_TCGNodes[i].first->m_BaseNode;
    }
}

