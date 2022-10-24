#include "bstar.h"

void BstarNode::m_copy(BstarNode* t_passive, BstarNode* t_active){
    if(!is_ecg){
        t_passive->set_x(t_active->x());
        t_passive->set_y(t_active->y());
        t_passive->set_w(t_active->w());
        t_passive->set_h(t_active->h());
        t_passive->set_r(t_active->r());
        t_passive->set_rcounter(t_active->rcounter());
        t_passive->set_left_child(t_active->left_child());
        t_passive->set_right_child(t_active->right_child());
        t_passive->set_parent(t_active->parent());
    }
}

void BstarNode::do_backup(){
    if(!is_ecg){
        m_copy(backup(), this);
    }
    else{
        back_num = current_num;
    }
}

void BstarNode::get_backup(){
    if(!is_ecg){
        m_copy(this, backup());
    }
    else{
        current_num = back_num;
    }
}

void BstarNode::restore_best(){
    if(!is_ecg){
        m_copy(best(), this);
    }
    else{
        best_num = current_num;
    }
}

void BstarNode::get_best(){
    if(!is_ecg){
        m_copy(this, best());
    }
    else{
        current_num = best_num;
    }
}

void  BstarNode::topology_transfer(vector<TCG*>& t_target, int t_root, int t_branch_num, general_node*& t_rootnode, vector<general_node*>& t_nodevec){
    topology_set.push_back(t_target);
    TCG* root = t_target[t_root];
    general_node* current_node;
    int current_index;
    general_node* root_node = new general_node(t_root);
    t_nodevec.push_back(root_node);
    set<pair<int, float>, pair_comparator> bottom_record;
    vector<bool> finish;
    finish.resize(t_target.size());
    for(int i=0; i<finish.size(); ++i){
        finish[i] = false;
    }
    int counter = 1;
    int counter_2 = 0;
    float similarity;
    queue<general_node*> trees;
    trees.push(root_node);
    while(counter < t_target.size()){
        current_node = trees.front();
        current_index = current_node->index;
        trees.pop();
        for(int i=0; i<t_target.size(); ++i){
            if(i != current_index && !finish[i]){
                counter_2 = 0;
                for(int j=0; j<t_target[i]->similarity_sequence.size(); ++j){
                    if(t_target[i]->similarity_sequence[j] == t_target[current_index]->similarity_sequence[j]){
                        counter_2++;
                    }
                }
                similarity = (float)counter_2/(float)(t_target[i]->m_TCGNodes.size()*2);
                bottom_record.insert(make_pair(i, similarity));
                if(bottom_record.size() > t_branch_num){
                    bottom_record.erase(prev(bottom_record.end()));
                }
            }
        }
        for(auto it=bottom_record.begin(); it!=bottom_record.end(); ++it){
            general_node* new_node = new general_node((*it).first);
            counter++;
            new_node->uppernode = current_node;
            current_node->bottom_nodes.push_back(new_node);
            trees.push(new_node);
            finish[(*it).first] = true;
            t_nodevec.push_back(new_node);
        }
        bottom_record.clear();
    }
    t_rootnode = root_node;
}
/////////////////// BstarTree/////////////////////

void BstarTree::m_TransformToBstarTree(vector<pair<float, float>> t_DieVec, 
                vector<vector<vector<TCG*>>>& t_ecgnode, 
                vector<pair<pair<float, float>, pair<float, float>>> t_CommonNetVec,
                vector<pair<int, int>> t_MappingCommonPinToDie
                ){
    string node_name;
    string pin_name_1;
    string pin_name_2;
    for(int i=0; i<t_DieVec.size(); ++i){
        node_name = "BstarNode"+to_string(i);
        BstarNode* new_node = new BstarNode(node_name, t_DieVec[i].first, t_DieVec[i].second);
        m_nodes.push_back(new_node);
    }
    for(int i=0; i<t_ecgnode.size(); ++i){
        node_name = "BstarNodeECG"+to_string(i);
        BstarNode* new_node = new BstarNode(node_name, t_ecgnode[i]);
        m_nodes.push_back(new_node);
        m_ecgs.push_back(new_node);
    }
    for(int i=0; i<t_CommonNetVec.size(); ++i){
        pin_name_1 = "BstarPin_node" + to_string(t_MappingCommonPinToDie[i].first) + "_" + to_string(2*i);
        pin_name_1 = "BstarPin_node" + to_string(t_MappingCommonPinToDie[i].second) + "_" + to_string(2*i+1);
        CommonBstarPin* new_pin_1 = new CommonBstarPin(pin_name_1, t_CommonNetVec[i].first.first, t_CommonNetVec[i].first.second);
        CommonBstarPin* new_pin_2 = new CommonBstarPin(pin_name_2, t_CommonNetVec[i].second.first, t_CommonNetVec[i].second.second);
        new_pin_1->set_dual_die(t_MappingCommonPinToDie[i].first);
        new_pin_2->set_dual_die(t_MappingCommonPinToDie[i].second);
        new_pin_1->set_die_pointer(&die_coors);
        new_pin_1->set_die_ratio(&die_ratios);
        new_pin_2->set_die_pointer(&die_coors);
        new_pin_2->set_die_ratio(&die_ratios);
        new_pin_1->set_dual_pin(new_pin_2);
        new_pin_2->set_dual_pin(new_pin_1);
        m_common_nets.push_back(make_pair(new_pin_1, new_pin_2));
    }
}


void BstarTree::m_xy_reset(){
    m_max_x = FLT_MIN;
    m_min_x = FLT_MAX;
    m_max_y = FLT_MIN;
    m_min_y = FLT_MAX;
}

void BstarTree::m_xy_update(float s_xcoor, float l_xcoor, float s_ycoor, float l_ycoor){
    if(s_xcoor < m_min_x){
        m_min_x = s_xcoor;
    }
    if(l_xcoor > m_max_x){
        m_max_x = l_xcoor;
    }
    if(s_ycoor < m_min_y){
        m_min_y = s_ycoor;
    }
    if(l_ycoor > m_max_y){
        m_max_y = l_ycoor;
    }
}

void BstarTree::m_get_coordinates(){
    m_xy_reset();
    stack<BstarNode*> node_stack;
    node_stack.push(m_root);
    BstarNode* lchild;
    BstarNode* rchild; 
    BstarNode* current_node;
    BstarNode* parent;
    float s_xcoor;
    float l_xcoor;
    float height;
    float max_height;
    float _x;
    float _y;
    float _min_y;
    float _max_y;
    float _new_y;
    TCG* current_tcg;
    while(node_stack.size() > 0){
        current_node = node_stack.top();
        node_stack.pop();
        lchild = current_node->left_child();
        rchild = current_node->right_child();
        parent = current_node->parent();
        pair<float, float> y_pair = make_pair(0,0);
        if(parent == m_ground){
            cout << "1??" << endl;
            if(current_node->is_ecg){
                cout << "2??" << endl;
                current_tcg = current_node->topology_set[current_node->current_num.first][current_node->current_num.second];
                vector<pair<float, float>> temp;
                vector<vector<float>>      temp_2;
                m_contour->find_segment(_x, _x + current_tcg->m_HCG->target()->value(), temp);
                cout << _x << "," << _x + current_tcg->m_HCG->target()->value() << "," << temp.size() << endl;
                current_tcg->insert_dummy_source(temp, _x, temp_2);
                current_node->set_x(0);
                current_node->set_w(current_tcg->m_HCG->target()->value());
                for(int i=0; i<temp_2.size(); ++i){
                    m_contour->insert_segment(temp_2[i][0], temp_2[i][1], temp_2[i][2], y_pair);
                    if(i > 0 && y_pair.first < _min_y){
                        _min_y = y_pair.first;
                    }
                    else if(i == 0){
                        _min_y = y_pair.first;
                    }
                    if(i > 0 && y_pair.second > _max_y){
                        _max_y = y_pair.second;
                    }
                    else if(i == 0){
                        _max_y = y_pair.second;
                    }
                }
                current_node->set_y(_min_y);
                s_xcoor = current_node->x();
                l_xcoor = current_node->l_x();
                max_height  = y_pair.second;
            }
            else{
                cout << "3??" << endl;
                current_node->set_x(0);
                current_node->set_y(0);
                s_xcoor = 0;
                _x = 0;
                _min_y = 0;
                l_xcoor = current_node->l_x();
                height = current_node->current_h();
                m_contour->insert_segment(s_xcoor, l_xcoor, height, y_pair);
                max_height = y_pair.second;                
            }
        }
        else{
            cout << "4??" << endl;
            if(parent->left_child() == current_node){
                _x = parent->l_x();
            }
            else{
                _x = parent->x();
            }
            if(current_node->is_ecg){
                cout << "ckk" << endl;
                current_tcg = current_node->topology_set[current_node->current_num.first][current_node->current_num.second];
                vector<pair<float, float>> temp;
                vector<vector<float>>      temp_2;
                cout << "crr" << endl;
                m_contour->find_segment(_x, _x + current_tcg->m_HCG->target()->value(), temp);
                cout << "rr" << endl;
                current_tcg->insert_dummy_source(temp, _x, temp_2);
                cout << "rr2" << endl;
                current_node->set_x(_x);
                current_node->set_w(current_tcg->m_HCG->target()->value());
                for(int i=0; i<temp_2.size(); ++i){
                    m_contour->insert_segment(temp_2[i][0], temp_2[i][1], temp_2[i][2], y_pair);
                    if(i > 0 && y_pair.first < _min_y){
                        _min_y = y_pair.first;
                    }
                    else if(i == 0){
                        _min_y = y_pair.first;
                    }
                    if(i > 0 && y_pair.second > _max_y){
                        _max_y = y_pair.second;
                    }
                    else if(i == 0){
                        _max_y = y_pair.second;
                    }
                }
                current_node->set_y(_min_y);
                s_xcoor = current_node->x();
                l_xcoor = current_node->l_x();
                max_height  = y_pair.second;
            }
            else{
                cout << "kk" << endl;
                current_node->set_x(_x);
                s_xcoor = current_node->x();
                l_xcoor = current_node->l_x();
                height = current_node->current_h();
                cout << "tt" << endl;
                m_contour->insert_segment(s_xcoor, l_xcoor, height, y_pair);
                cout << "tt2" << endl;
                _min_y = y_pair.first;
                current_node->set_y(_min_y);
                max_height = y_pair.second;
            }
        }
        m_xy_update(s_xcoor, l_xcoor, _min_y, max_height);
        if(rchild != m_ground){
            node_stack.push(rchild);
        }
        if(lchild != m_ground){
            node_stack.push(lchild);
        }
        cout << "wwww" << endl;
    }
    m_contour->reset();
    cout << "qqqqq" << endl;
}

void BstarTree::m_initialize(){
    queue<BstarNode*> node_queue;
    bool period = true;
    BstarNode* current_node = m_nodes.at(0);
    m_root = m_nodes.at(0);
    for(unsigned i=0; i<m_nodes.size(); ++i){
        m_nodes.at(i)->set_left_child(m_ground);
        m_nodes.at(i)->set_right_child(m_ground);
        m_nodes.at(i)->set_parent(m_ground);
    }
    for(unsigned i=1; i<m_nodes.size(); ++i){
        if(period){
            current_node->set_left_child(m_nodes.at(i));
        }
        else{
            current_node->set_right_child(m_nodes.at(i));
        }
        m_nodes.at(i)->set_parent(current_node);
        period = !period;
        node_queue.push(m_nodes.at(i));
        if(period){
            current_node = node_queue.front();
            node_queue.pop();
        }
    }
    cout << "get coor" << endl;
    m_get_coordinates();
}

void BstarTree::do_backups(){
    for(int i=0; i<m_nodes.size(); ++i){
        m_nodes.at(i)->do_backup();
    }
    m_max_x_backup = m_max_x;
    m_max_y_backup = m_max_y;
    m_min_x_backup = m_min_x;
    m_min_y_backup = m_min_y;
    m_root_backup = m_root;
}

void BstarTree::get_backups(){
    for(int i=0; i<m_nodes.size(); ++i){
        m_nodes.at(i)->get_backup();
    }
    m_max_x = m_max_x_backup;
    m_max_y = m_max_y_backup;
    m_min_x = m_min_x_backup;
    m_min_y = m_min_y_backup;
    m_root = m_root_backup;
}

void BstarTree::restore_bests(){
    for(int i=0; i<m_nodes.size(); ++i){
        m_nodes.at(i)->restore_best();
    }
    m_max_x_best = m_max_x;
    m_max_y_best = m_max_y;
    m_min_x_best = m_min_x;
    m_min_y_best = m_min_y;
    m_root_best = m_root;
}

void BstarTree::get_bests(){
    for(int i=0; i<m_nodes.size(); ++i){
        m_nodes.at(i)->get_best();
    }
    m_max_x = m_max_x_best;
    m_max_y = m_max_y_best;
    m_min_x = m_min_x_best;
    m_min_y = m_min_y_best;
    m_root = m_root_best;
}

float BstarTree::area(){
    return (m_max_x - m_min_x)*(m_max_y - m_min_y);
}

float BstarTree::HPWL(){
    float total_hpwl = 0;
    for(int i=0; i<m_common_nets.size(); ++i){
        total_hpwl += abs(m_common_nets[i].first->x() - m_common_nets[i].second->x()) + abs(m_common_nets[i].first->y() - m_common_nets[i].second->y());
    }
    return total_hpwl;
}

void BstarTree::swap_move(BstarNode* node_1, BstarNode* node_2){
    if(m_root == node_1){
        m_root = node_2;
    }
    else if(m_root == node_2){
        m_root = node_1;
    }
    BstarNode* node_1_parent = node_1->parent();
    BstarNode* node_1_lchild = node_1->left_child();
    BstarNode* node_1_rchild = node_1->right_child();
    BstarNode* node_2_parent = node_2->parent();
    BstarNode* node_2_lchild = node_2->left_child();
    BstarNode* node_2_rchild = node_2->right_child();
    if(node_1_parent == node_2){
        node_2->set_parent(node_1);
        node_1->set_parent(node_2_parent);
        if(node_1_parent->left_child() == node_1){
            node_1->set_left_child(node_2);
            node_1->set_right_child(node_2_rchild);
            node_2_rchild->set_parent(node_1);
        }
        else{
            node_1->set_right_child(node_2);
            node_1->set_left_child(node_2_lchild);
            node_2_lchild->set_parent(node_1);
        }
        if(node_2_parent->left_child() == node_2){
            if(node_2_parent != m_ground){
                node_2_parent->set_left_child(node_1);
            }
        }
        else{
            if(node_2_parent != m_ground){
                node_2_parent->set_right_child(node_1);
            }
        }
        node_2->set_left_child(node_1_lchild);
        node_2->set_right_child(node_1_rchild);
        node_1_lchild->set_parent(node_2);
        node_1_rchild->set_parent(node_2);
    }
    else if(node_2_parent == node_1){
        node_1->set_parent(node_2);
        node_2->set_parent(node_1_parent);
        if(node_2_parent->left_child() == node_2){
            node_2->set_left_child(node_1);
            node_2->set_right_child(node_1_rchild);
            node_1_rchild->set_parent(node_2);
        }
        else{
            node_2->set_right_child(node_1);
            node_2->set_left_child(node_1_lchild);
            node_1_lchild->set_parent(node_2);
        }
        if(node_1_parent->left_child() == node_1){
            if(node_1_parent != m_ground){
                node_1_parent->set_left_child(node_2);
            }
        }
        else{
            if(node_1_parent != m_ground){
                node_1_parent->set_right_child(node_2);
            }
        }
        node_1->set_left_child(node_2_lchild);
        node_1->set_right_child(node_2_rchild);
        node_2_lchild->set_parent(node_1);
        node_2_rchild->set_parent(node_1);
    }
    else{
        node_1->set_left_child(node_2_lchild);
        node_1->set_right_child(node_2_rchild);
        node_1->set_parent(node_2_parent);
        node_2->set_left_child(node_1_lchild);
        node_2->set_right_child(node_1_rchild);
        node_2->set_parent(node_1_parent);
        if(node_1_parent->left_child() == node_1){
            if(node_1_parent != m_ground){
                node_1_parent->set_left_child(node_2);
            }
        }
        else{
            if(node_1_parent != m_ground){
                node_1_parent->set_right_child(node_2);
            }
        }
        if(node_2_parent->left_child() == node_2){
            if(node_2_parent != m_ground){
                node_2_parent->set_left_child(node_1);
            }
        }
        else{
            if(node_2_parent != m_ground){
                node_2_parent->set_right_child(node_1);
            }
        }
        node_1_lchild->set_parent(node_2);
        node_1_rchild->set_parent(node_2);
        node_2_lchild->set_parent(node_1);
        node_2_rchild->set_parent(node_1);
    }
}

void BstarTree::rotate(BstarNode* t_node){
    int roatate_index = t_node->r()+1;
    if(roatate_index == 4){
        roatate_index = 0;
    }
    t_node->set_r(roatate_index);
}

void BstarTree::delete_insert(BstarNode* d_node, BstarNode* i_node, bool dim){
    BstarNode* l_r;
    BstarNode* child;
    while((d_node->left_child()!=m_ground)||(d_node->right_child()!=m_ground)){
        if(d_node->left_child() != m_ground){
            l_r = d_node->left_child();
        }
        else{
            l_r = d_node->right_child();
        }
        swap_move(d_node, l_r);
    }
    if(d_node->parent()->left_child() == d_node){
        d_node->parent()->set_left_child(m_ground);
    }
    else{
        d_node->parent()->set_right_child(m_ground);
    }

    if(dim){
        child = i_node->right_child();
        i_node->set_right_child(d_node);
        d_node->set_right_child(child);
    }
    else{
        child = i_node->left_child();
        i_node->set_left_child(d_node);
        d_node->set_left_child(child);
    }
    d_node->set_parent(i_node);
    if(child != m_ground){
        child->set_parent(d_node);
    }
}

void BstarTree::transfer_topology(BstarNode* t_node, int t_change_group, bool t_dir, int t_step){
    bool dir = t_dir;
    general_node* original_node = t_node->current_node;
    if(t_change_group != t_node->current_num.first){
        cout << "gg" << endl;
        t_node->current_num.first = t_change_group;
        cout << t_node->node_record[t_change_group].size() << endl;
        t_node->current_node = t_node->node_record[t_change_group][(rand()%t_node->node_record[t_change_group].size())];
        t_node->current_num.second = t_node->current_node->index;
    }
    else{
        int counter = 0;
        cout << "ggc" << endl;
        while(counter < t_step){
            cout << "ggc0" << endl;
            if(dir && t_node->current_node->bottom_nodes.size() > 0){
                cout << "ggc1" << endl;
                t_node->current_node = t_node->current_node->bottom_nodes[rand()%t_node->current_node->bottom_nodes.size()];
                cout << "ggc2" << endl;
                t_node->current_topology = t_node->topology_set[t_node->current_num.first][t_node->current_node->index];
                counter++;
            }
            else if(!dir && t_node->current_node->uppernode != 0){
                cout << "ggc3" << endl;
                t_node->current_node = t_node->current_node->uppernode;
                cout << "ggc4" << endl;
                t_node->current_topology = t_node->topology_set[t_node->current_num.first][t_node->current_node->index];
                counter++;
            }
            else{
                if(t_node->current_node == original_node){
                    dir = !dir;
                }
                else{
                    counter = t_step;
                }
            }
        }
    }
    cout << "yy" << endl;
}

vector<float> BstarTree::get_dies_coor(int t_die_index){
    vector<float> die_inf;
    BstarNode* target_node = m_die_map[t_die_index];
    die_inf.push_back(target_node->x());
    die_inf.push_back(target_node->y()); 
    die_inf.push_back(target_node->r());
    return die_inf;
}

void BstarTree::write_output(char* arg){
    ofstream bgraph;
    bgraph.open(arg);
    BstarNode* n;
    for(unsigned i=0; i<m_nodes.size(); ++i){
        BstarNode* n = m_nodes.at(i);
        bgraph << i << " "<<n->x() << " "<< n->y() << " "<< n->current_w() <<
        " "<< n->current_h() << endl;
    }
    bgraph.close();
}

