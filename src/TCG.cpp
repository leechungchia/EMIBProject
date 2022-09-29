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
    return revised;
}
float TCGNode::Overlap(TCGNode* t_node){
    float value_1 = m_value;
    float max_1   = m_value+m_weight;
    float value_2 = t_node->value();
    float max_2   = value_2+t_node->weight();
    //cout << CodeName() << "," << t_node->CodeName() <<">>>>>>>>>>>>>"<< value_1 << max_1 << value_2 << max_2 << endl;
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
}




bool EMIBNet::isOverlapValid(float& t_overlap){
    t_overlap = m_node_1->Overlap(m_node_2);
    return (m_node_1->Overlap(m_node_2) >= m_overlap);
}
bool EMIBNet::isDistanceValid(){
    return (m_node_1->Distance(m_node_2) <= m_distance);
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
        if(current_node->finished()){
            current_node->ValueGenerate();
        }
        if(current_node->EMIBCounter() > 0){
            newToppestNodes.insert(current_node);
        }
        else{
            current_node->SetVisited();
            for(auto it = current_node->DirectUpperNodes()->begin(); it!=current_node->DirectUpperNodes()->end(); ++it){
                if((*it)->finished()){
                    traverseQueue.push((*it));
                }
            }
        }
    }
    for(auto it=newToppestNodes.begin(); it!=newToppestNodes.end(); ++it){
        vector<EMIBNet*> temp;
        for(int i=0; i<(*it)->EMIBs.size(); ++i){
            if(newToppestNodes.find((*it)->EMIBs[i]->dualnode((*it))) != newToppestNodes.end()){
                temp.push_back((*it)->EMIBs[i]);
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
    vector<TCGNode*> bound;
    vector<EMIBP*>   bound_EMIBs;
    vector<EMIBP*> belowEMIBset;
    bound.push_back(m_source);
    bool graph_state = state();
    bool           bound_success;
    bool           legalization_success;
    bound_success = m_TraverseToBound(bound, bound_EMIBs);
    cout << m_direction_type << " Depth: " << m_EMIBdepth << endl;
    cout << "Toppest EMIB num: " << bound_EMIBs.size() << endl;
    for(int i=0; i<bound.size(); ++i){
        cout << bound[i]->CodeName() << ",";
    }
    cout << endl; 
    while(bound_success && bound_EMIBs.size() > 0){
        for(int i=0; i<bound_EMIBs.size(); ++i){
            cout << m_direction_type << "Depth " << m_EMIBdepth << " EMIB " << i << endl;
            legalization_success = legal->Legalization(bound_EMIBs[i]);
            if(!legalization_success){
                return 0;
            }
        }
        for(int i=0; i<bound_EMIBs.size(); ++i){
            bound_EMIBs[i]->half_Initialize();
        }
        bound_success = m_TraverseToBound(bound, bound_EMIBs);
        cout <<  m_direction_type << "Depth: " << m_EMIBdepth << endl;
        cout << "Toppest EMIBP num: " << bound_EMIBs.size() << endl; 
    }
    if(!bound_success){
        m_EMIBdepth = 0;
        return 0;
    }
    for(auto it=m_source->UpperNodes()->begin(); it!=m_source->UpperNodes()->end(); ++it){
        cout << (*it)->CodeName() << " x : " << (*it)->value() << endl; 
    }
    m_EMIBdepth = 0;
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
void TCGGraph::m_CoorGenerate(){
}
void TCGGraph::Initialize(vector<TCGNode*>* t_TCGNodes, bool t_is_activated){
    m_source->BottomNodes()->clear();
    m_source->UpperNodes()->clear();
    m_target->BottomNodes()->clear();
    m_target->UpperNodes()->clear();
    m_TCGNodes = t_TCGNodes;
    for(int i=0; i<t_TCGNodes->size(); ++i){
        t_TCGNodes->at(i)->ResetCounter();
    }
    m_source->ResetCounter();
    m_target->ResetCounter();
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
    }
    for(int i=0; i<m_EMIBInf->size(); ++i){
        node_1 = t_TCGNodes->at(m_EMIBInf->at(i).node_1);
        node_2 = t_TCGNodes->at(m_EMIBInf->at(i).node_2);
        cout << node_1->CodeName() << "," << node_2->CodeName() << endl;
        if(node_1->UpperNodes()->find(node_2) == node_1->UpperNodes()->end() && node_2->UpperNodes()->find(node_1) == node_2->UpperNodes()->end()){
            code_name = to_string(m_EMIBInf->at(i).node_1) + "_" + to_string(m_EMIBInf->at(i).node_2);
            overlap = m_EMIBInf->at(i).overlap;
            distance = m_EMIBInf->at(i).distance;
            occupied = m_EMIBInf->at(i).occupied;
            EMIBNet* net = new EMIBNet(code_name, node_1, node_2, overlap, distance, occupied);
            EMIBNet* net_2 = new EMIBNet(code_name, node_2, node_1, overlap, distance, occupied);
            m_Nets[m_EMIBInf->at(i).node_1].push_back(net);
            t_TCGNodes->at(m_EMIBInf->at(i).node_1)->EMIBs.push_back(net);
            m_Nets[m_EMIBInf->at(i).node_2].push_back(net_2);
            t_TCGNodes->at(m_EMIBInf->at(i).node_2)->EMIBs.push_back(net_2);
        }
    }
}
void TCGGraph::ConstraintEdgeAdd(vector<vector<int>>& t_edges){
    for(int i=0; i<t_edges.size(); ++i){
        m_TCGNodes->at(t_edges[i][0])->UpperInsert(m_TCGNodes->at(t_edges[i][1]));
        m_TCGNodes->at(t_edges[i][1])->BottomInsert(m_TCGNodes->at(t_edges[i][0]));
    }
}

void TCGGraph::DirectEdgeAdd(){
    bool above_exist = true;
    TCGNode* current_above_node;
    TCGNode* current_node;
    for(int i=0; i<m_TCGNodes->size(); ++i){
        current_node = m_TCGNodes->at(i);
        for(auto it=current_node->UpperNodes()->begin(); it!=current_node->UpperNodes()->end(); ++it){
            current_above_node = (*it);
            above_exist = true;
            for(auto it_2=current_node->UpperNodes()->begin(); it_2!=current_node->UpperNodes()->end(); ++it_2){
                if((it!=it_2) && ( (*it_2)->UpperNodes()->find((*it)) != (*it_2)->UpperNodes()->end())){
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
    for(auto it=m_target->BottomNodes()->begin(); it!=m_target->BottomNodes()->end(); ++it){
        current_above_node = (*it);
        above_exist = true;
        for(auto it_2=m_target->BottomNodes()->begin(); it_2!=m_target->BottomNodes()->end(); ++it_2){
            if((it!=it_2) && ( (*it_2)->BottomNodes()->find((*it)) != (*it_2)->BottomNodes()->end())){
                above_exist = false;
                break;
            }
        }
        if(above_exist){
            m_target->DirectBottomNodes()->insert(current_above_node);
            current_above_node->DirectUpperNodes()->insert(m_target);
        }
    }
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
    for(auto it=t_node->DirectUpperNodes()->begin(); it!=t_node->DirectUpperNodes()->end(); ++it){
        if((*it)->ValueUpdate()){
            t_NodeSet.insert((*it));
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
        t_UpdateNodes.insert(low);
        CheckViolation(CorrectSet, new_WrongSet);
        while(new_WrongSet.size() > 0){
            (*new_WrongSet.begin())->LowHighDerive(high, low);
            SetLegal((*new_WrongSet.begin()));
            t_UpdateNodes.insert(low);
            CheckViolation(CorrectSet, new_WrongSet_2);
            new_WrongSet.erase(new_WrongSet.begin());
            new_WrongSet.insert(new_WrongSet_2.begin(), new_WrongSet_2.end());
            new_WrongSet_2.clear();
        }
        CorrectSet.clear();
        WrongSet.clear();
        FindIllegal(t_EMIBP, WrongSet, CorrectSet);
    }
}
void Legalizer::SelfUpdate(TCGNode* t_node, set<TCGNode*>& t_UpdateNodeSet){
    set<EMIBP*, EMIBP_comparator> EMIBPSet;
    EMIBPSet.insert(t_node->EMIBPs.begin(), t_node->EMIBPs.end());
    UpdateAbove(t_node, t_UpdateNodeSet);
    for(auto it=EMIBPSet.begin(); it!=EMIBPSet.end(); ++it){
        EMIBPUpdate((*it), t_UpdateNodeSet);
    }
}
bool Legalizer::BranchUpdate(EMIBNet* t_EMIBNet, TCGNode* t_high, TCGNode* t_low, set<EMIBNet*, EMIBNet_comparator>& t_newWrongSet, pair<TCGNode*, TCGNode*>& t_target, set<EMIBNet*, EMIBNet_comparator>& t_CorrectSet, bool t_state){
    set<TCGNode*> UpdateNodes;
    set<TCGNode*> new_UpdateNodes;;
    bool overlap_success = SetLegal(t_EMIBNet);
    if(!overlap_success){
        return 0;
    }
    SelfUpdate(t_low, UpdateNodes);
    set<TCGNode*, node_comparator> ProcessOrder;
    TCGNode*      current_node;
    TCGNode*      high;
    TCGNode*      low;
    float         overlap;
    ProcessOrder.insert(UpdateNodes.begin(), UpdateNodes.end());
    while(ProcessOrder.size() > 0){
        //cout << "BranchUpdate ProcessOrder: "<< (*ProcessOrder.begin())->CodeName() << endl;
        current_node = (*ProcessOrder.begin());
        SelfUpdate(current_node, new_UpdateNodes);
        ProcessOrder.erase(ProcessOrder.begin());
        ProcessOrder.insert(new_UpdateNodes.begin(), new_UpdateNodes.end());
        new_UpdateNodes.clear();
    }
    if(!t_EMIBNet->isOverlapValid(overlap)){
        cout << "unlegalizable branch: " << t_EMIBNet->node1()->CodeName() << ","  << t_EMIBNet->node2()->CodeName() << endl;
        return 0;
    }
    else{
        CheckViolation(t_CorrectSet, t_newWrongSet);
        if(t_state){
            t_target.first = high;
            t_target.second = low;
        }
        else{
            for(auto it=t_newWrongSet.begin(); it!=t_newWrongSet.end(); ++it){
                (*it)->LowHighDerive(high, low);
                if(high == t_target.first && low == t_target.second){
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
    bool success = BranchUpdate(t_EMIBNet, t_high, t_low, newWrongSet, target, t_CorrectSet, 1);
    //cout << "BranchUpdate: " << success << endl;
    if(!success){
        return 0;
    }
    while(newWrongSet.size() > 0){
        current_net = (*newWrongSet.begin());
        current_net->LowHighDerive(high, low);
        success = BranchUpdate(current_net, high, low, newWrongSet_2, target, t_CorrectSet, 0);
        if(!success){
            return 0;
        }
        newWrongSet.erase(newWrongSet.begin());
        newWrongSet.insert(newWrongSet_2.begin(), newWrongSet_2.end());
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
    cout << "Legalization Iteration 0" << endl;
    cout << "Wrong EMIB num: " << WrongSet.size() << endl;
    while(WrongSet.size() > 0){
        FindCritical(current_net, high, low, WrongSet);
        success = CompleteUpdate(current_net, high, low, CorrectSet, WrongSet);
        if(!success){
            return 0;
        }
        FindIllegal(t_EMIBP, WrongSet, CorrectSet);
        iteration++;
        cout << "Legalization Iteration " << iteration << endl;
        cout << "Wrong EMIB num: " << WrongSet.size() << endl;
    }
    return 1;
}


void TCG::GetTCGEdge(vector<vector<int>> t_h_edges, vector<vector<int>> t_v_edges){
    m_HCG->ConstraintEdgeAdd(t_h_edges);
    m_HCG->DirectEdgeAdd();
    m_VCG->ConstraintEdgeAdd(t_v_edges);
    m_VCG->DirectEdgeAdd();
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
}
void TCG::Initialize(){
    for(int i=0; i<m_HCGNodes.size(); ++i){
        m_HCGNodes[i]->HardInitialize();
    }
    for(int i=0; i<m_VCGNodes.size(); ++i){
        m_VCGNodes[i]->HardInitialize();
    }
    m_HCG->SetEMIBInf(&m_EMIBInfs);
    m_VCG->SetEMIBInf(&m_EMIBInfs);
    cout << "Read TCG EMIB Inf" << endl;
    m_HCG->EMIBNetDerive(&m_HCGNodes);
    m_VCG->EMIBNetDerive(&m_VCGNodes);
    for(int i=0; i<m_HCGNodes.size(); ++i){
        m_HCGNodes[i]->SoftInitialize();
    }
    for(int i=0; i<m_VCGNodes.size(); ++i){
        m_VCGNodes[i]->SoftInitialize();
    }
    cout << "HCG EMIB num: "<< m_HCG->EMIBnum() << endl;
    cout << "VCG EMIB num: "<< m_VCG->EMIBnum() << endl;
    cout << "Classify EMIB net successfully" << endl;
    //string h_success = (m_HCG->Overlap_Legalization())?("success"):("false");
    //string v_success = (m_VCG->Overlap_Legalization())?("success"):("false");
    //cout << "HCG Legalization: " << h_success << endl;
    //cout << "VCG Legalization: " << v_success << endl;
}
vector<float> TCG::get_dies_coor(int t_die_index){
    vector<float> die_inf;
    TCGNode* target_node = m_die_map[t_die_index];
    die_inf.push_back(target_node->value());
    die_inf.push_back(target_node->DualNode()->value()); 
    die_inf.push_back(target_node->r());
    return die_inf;
}