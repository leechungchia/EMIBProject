#include "bstar.h"

void BstarNode::m_copy(BstarNode* t_passive, BstarNode* t_active){
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

void BstarNode::do_backup(){
    m_copy(backup(), this);
    
}

void BstarNode::get_backup(){
    m_copy(this, backup());
}

void BstarNode::restore_best(){
    m_copy(best(), this);
}

void BstarNode::get_best(){
    m_copy(this, best());
}

/////////////////// BstarTree/////////////////////

void BstarTree::m_TransformToBstarTree(vector<pair<float, float>>& t_NodeVec, vector<pair<pair<float, float>, pair<float, float>>>& t_PinVec, vector<pair<int, int>>& t_PinNodeMap){
    string node_name;
    string pin_name_1;
    string pin_name_2;
    for(int i=0; i<t_NodeVec.size(); ++i){
        node_name = "BstarNode"+to_string(i);
        BstarNode* new_node = new BstarNode(node_name, t_NodeVec[i].first, t_NodeVec[i].second);
        m_nodes.push_back(new_node);
        m_die_map[i] = new_node;
    }
    for(int i=0; i<t_PinVec.size(); ++i){
        pin_name_1 = "BstarPin_node" + to_string(t_PinNodeMap[i].first) + "_" + to_string(2*i);
        pin_name_1 = "BstarPin_node" + to_string(t_PinNodeMap[i].second) + "_" + to_string(2*i+1);
        CommonBstarPin* new_pin_1 = new CommonBstarPin(pin_name_1, t_PinVec[i].first.first, t_PinVec[i].first.second);
        CommonBstarPin* new_pin_2 = new CommonBstarPin(pin_name_2, t_PinVec[i].second.first, t_PinVec[i].second.second);
        new_pin_1->set_dual_pin(new_pin_2);
        new_pin_2->set_dual_pin(new_pin_1);
        new_pin_1->set_BstarNode(m_nodes.at(t_PinNodeMap[i].first));
        new_pin_2->set_BstarNode(m_nodes.at(t_PinNodeMap[i].second));
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
    float _x;
    float _y;
    while(node_stack.size() > 0){
        current_node = node_stack.top();
        node_stack.pop();
        lchild = current_node->left_child();
        rchild = current_node->right_child();
        parent = current_node->parent();
        if(parent == m_ground){
            current_node->set_x(0);
            current_node->set_y(0);
            s_xcoor = 0;
            _x = 0;
            _y = 0;
            l_xcoor = current_node->l_x();
            height = current_node->current_h();
            m_contour->insert_segment(s_xcoor, l_xcoor, height);
        }
        else{
            if(parent->left_child() == current_node){
                _x = parent->l_x();
            }
            else{
                _x = parent->x();
            }
            current_node->set_x(_x);
            s_xcoor = current_node->x();
            l_xcoor = current_node->l_x();
            height = current_node->current_h();
            _y = m_contour->insert_segment(s_xcoor, l_xcoor, height);
            current_node->set_y(_y);
        }
        m_xy_update(s_xcoor, l_xcoor, _y, _y+height);
        if(rchild != m_ground){
            node_stack.push(rchild);
        }
        if(lchild != m_ground){
            node_stack.push(lchild);
        }
    }
    m_contour->reset();
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