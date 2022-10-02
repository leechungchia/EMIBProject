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
        cout << "EMIB input error" << endl;
    }
    string name;
    string die_1;
    string die_2;
    float  overlap;
    float  distance;
    float  occupied;
    while(data >> name)
    {
        data >> die_1;
        data >> die_2;
        data >> overlap;
        data >> distance;
        data >> occupied;
        EMIB* temp = new EMIB(m_search_die(die_1), m_search_die(die_2), overlap, distance, occupied);
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

void GlobalPlacer::m_tree_node_generation(vector<die*>& t_dies, vector<EMIB*>& t_EMIBs, vector<tree_net*>& t_treenets){
    tree_node* node1;
    tree_node* node2;
    for(int i=0; i<t_EMIBs.size(); ++i){
        node1 = t_dies[t_EMIBs[i]->die_1]->dual_treenode;
        node2 = t_dies[t_EMIBs[i]->die_2]->dual_treenode;
        tree_net* new_net = new tree_net(node1, node2);
        t_treenets.push_back(new_net);
    }
}

void GlobalPlacer::m_random_net_generate(int num)
{
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

void GlobalPlacer::m_random_die_generate(int t_num, pair<int, int> t_width_range, pair<int, int> t_height_range){
    float w;
    float h;
    for(int i=0; i<t_num; ++i){
        w = (t_width_range.second - t_width_range.first > 0)?(rand()%(t_width_range.second - t_width_range.first) + t_width_range.first):(t_width_range.first);
        h = (t_height_range.second - t_height_range.first > 0)?(rand()%(t_height_range.second - t_height_range.first) + t_height_range.first):(t_height_range.first);
        die* r_die = new die("randomdie"+to_string(i), w, h, i);
        m_DieVec.push_back(r_die);
    }
}

void GlobalPlacer::m_random_EMIB_generate(int t_num, pair<int, int> t_overlap_range, pair<int, int> t_distance_range, pair<int, int> t_occupied_range){
    int die_1;
    int die_2;
    float o;
    float d;
    float oc;
    bool redundant=true;
    for(int i=0; i<t_num; ++i){
        redundant = true;
        while(redundant){
            die_1 = rand()%m_DieVec.size();
            die_2 = rand()%m_DieVec.size();
            while(die_2 == die_1){
                die_2 = rand()%m_DieVec.size();
            }
            if(m_EMIBNets.size() == 0){
                redundant = false;
            }
            for(int j=0; j<m_EMIBNets.size(); ++j){
                if((die_1 == m_EMIBNets[j]->die_1 && die_2 == m_EMIBNets[j]->die_2)|| (die_2 == m_EMIBNets[j]->die_1 && die_1 == m_EMIBNets[j]->die_2)){
                    break;
                }
                if(j == m_EMIBNets.size()-1){
                    redundant = false;
                }
            }
        }
        o = (t_overlap_range.second - t_overlap_range.first > 0)?(rand()%(t_overlap_range.second - t_overlap_range.first) + t_overlap_range.first):(t_overlap_range.first);
        d = (t_distance_range.second - t_distance_range.first > 0)?(rand()%(t_distance_range.second - t_distance_range.first) + t_distance_range.first):(t_distance_range.first);
        oc = (t_occupied_range.second - t_occupied_range.first > 0)?(rand()%(t_occupied_range.second - t_occupied_range.first) + t_occupied_range.first):(t_occupied_range.first);
        while((m_DieVec[die_1]->w() < o && m_DieVec[die_1]->h() < o) || (m_DieVec[die_2]->w() < o && m_DieVec[die_2]->h() < o)){
            o = (t_overlap_range.second - t_overlap_range.first > 0)?(rand()%(t_overlap_range.second - t_overlap_range.first) + t_overlap_range.first):(t_overlap_range.first);
        }
        while((m_DieVec[die_1]->w() < oc && m_DieVec[die_1]->h() < oc) || (m_DieVec[die_2]->w() < oc && m_DieVec[die_2]->h() < oc)){
            (t_occupied_range.second - t_occupied_range.first > 0)?(rand()%(t_occupied_range.second - t_occupied_range.first) + t_occupied_range.first):(t_occupied_range.first);
        }
        EMIB* emib = new EMIB(die_1, die_2, o, d, oc);
        m_EMIBNets.push_back(emib);
    }
}


void GlobalPlacer::m_findroot(tree_node*& t_node, tree_node*& t_root){
    tree_node* current_node;
    current_node = t_node;
    vector<tree_node*> updatenodes;
    while(current_node->upper_node != 0){
        updatenodes.push_back(current_node);
        current_node = current_node->upper_node;
        cout << current_node << endl;
    }
    t_root = current_node;
    for(int i=0; i<updatenodes.size(); ++i){
        updatenodes[i]->upper_node = current_node;
    }
}


bool GlobalPlacer::m_unioninsert(tree_net* t_net, vector<vector<die*>>& t_dietrees){
    tree_node* node1;
    tree_node* node2;
    tree_node* root1;
    tree_node* root2;
    node1 = t_net->die1;
    node2 = t_net->die2;
    if(node1->inserted){
        m_findroot(node1, root1);
    }
    if(node2->inserted){
        m_findroot(node2, root2);
    }
    if(node1->inserted && node2->inserted){
        if(root1 == root2){
            return 0;
        }
        else{
            root2->upper_node = root1;
        }
    }
    else if(node1->inserted && !node2->inserted){
        node2->upper_node = root1;
        node2->inserted = true;
    }
    else if(node2->inserted && !node1->inserted){
        node1->upper_node = root2;
        node1->inserted = true;
    }
    else{
        node1->upper_node = 0;
        node2->upper_node = node1;
        node1->inserted = true;
        node2->inserted = true;
    }
    t_dietrees[node1->dual_die->initial_index()].push_back(node2->dual_die);
    t_dietrees[node2->dual_die->initial_index()].push_back(node1->dual_die);
    return 1;
}
void GlobalPlacer::m_maximum_spanning_tree(vector<die*>& t_dies, vector<EMIB*>& t_nets, MST_node*& t_root){
    if(t_dies.size() == 1){
        t_root = t_dies[0]->dual_MSTnode;
        return;
    }
    vector<tree_net*> treenets;
    for(int i=0; i<t_nets.size(); ++i){
        tree_net* new_net = new tree_net(t_dies[t_nets[i]->die_1]->dual_treenode, t_dies[t_nets[i]->die_2]->dual_treenode);
        treenets.push_back(new_net);
    }
    vector<vector<die*>> MSTtrees;
    for(int i=0; i<t_dies.size(); ++i){
        vector<die*> temp;
        MSTtrees.push_back(temp);
    }
    int net_num = 0;
    int iterator = 0;
    while(net_num < t_dies.size()-1){
        if(m_unioninsert(treenets[iterator], MSTtrees)){
            net_num++;
        };
        iterator++;
    }
    queue<MST_node*> MSTnodes;
    int root_index = rand()%t_dies.size();
    MST_node* root = t_dies[root_index]->dual_MSTnode;
    t_root = root;
    MST_node* current_node;
    MST_node* bottom_node;
    MSTnodes.push(root);
    while(MSTnodes.size() > 0){
        current_node = MSTnodes.front();
        MSTnodes.pop();
        current_node->visited = true;
        for(int i=0; i<MSTtrees[current_node->dual_die->initial_index()].size(); ++i){
            bottom_node = MSTtrees[current_node->dual_die->initial_index()][i]->dual_MSTnode;
            if(!bottom_node->visited){
                MSTnodes.push(bottom_node);
                current_node->bottom_nodes.push_back(bottom_node);
            }
        }
    }
    cout << "MST tree constructed successfully" << endl;
}

void GlobalPlacer::m_reduction_assignment(MST_node* t_root, vector<MST_node*> t_aboveroots,vector<vector<int>>& t_edges){
    for(int i=0; i<t_root->bottom_nodes.size(); ++i){
        vector<MST_node*> aboveroots(t_aboveroots);
        vector<int> r_edge;
        r_edge.push_back(t_root->dual_die->initial_index2());
        r_edge.push_back(t_root->bottom_nodes[i]->dual_die->initial_index2());
        r_edge.push_back(1);
        t_edges.push_back(r_edge);
        for(int j=0; j<aboveroots.size(); ++j){
            vector<int> edge;
            edge.push_back(aboveroots[j]->dual_die->initial_index2());
            edge.push_back(t_root->bottom_nodes[i]->dual_die->initial_index2());
            edge.push_back(0);
            t_edges.push_back(edge);
        }
        aboveroots.push_back(t_root);
        m_reduction_assignment(t_root->bottom_nodes[i], aboveroots, t_edges);
    }
}

void GlobalPlacer::m_random_assignment(MST_node* t_root , vector<MST_node*>& t_bottom, vector<vector<int>>& t_edges){
    int root_size = t_root->bottom_nodes.size();
    int input_edge_num;
    int output_edge_num;
    vector<int> root_order;
    vector<vector<MST_node*>> bottom;
    vector<vector<int>> edges;
    for(int i=0; i<root_size; ++i){
        root_order.push_back(i);
    }
    random_shuffle(root_order.begin(), root_order.end());
    for(int i=0; i<root_order.size(); ++i){
        vector<MST_node*> sub_bottom;
        if(t_root->bottom_nodes[root_order[i]]->bottom_nodes.size() != 0){
            m_random_assignment(t_root->bottom_nodes[root_order[i]], sub_bottom, t_edges);
        }
        else{
            sub_bottom.push_back(t_root->bottom_nodes[root_order[i]]);
        }
        bottom.push_back(sub_bottom);
    }
    int in_out;
    map<int, set<int>> edge_map;
    for(int i=0; i<bottom.size(); ++i){
        set<int> temp;
        edge_map[i] = temp;
    }
    if(bottom.size() > 1){
        for(int i=0; i<bottom.size(); ++i){
            set<int> in;
            set<int> out;
            in_out=rand()%2;
            if(in_out == 0){
                input_edge_num = rand()%(bottom.size()-i);
                output_edge_num = bottom.size() - i -1 - input_edge_num;
            }
            else{
                output_edge_num = rand()%(bottom.size()-i);
                input_edge_num = bottom.size() - i- 1 - output_edge_num;
            }
            for(int j=i+1; j<input_edge_num+i+1; ++j){
                if(edge_map[i].find(j) == edge_map[i].end()){
                    for(int z=0; z<bottom[i].size(); ++z){
                        for(int w=0; w<bottom[j].size(); ++w){
                            vector<int> edge;
                            edge.push_back(bottom[j][w]->dual_die->initial_index2());
                            edge.push_back(bottom[i][z]->dual_die->initial_index2());
                            edge.push_back(1);
                            edges.push_back(edge);
                            in.insert(j);
                        }
                    }
                }
            }
            for(int j=i+input_edge_num+1; j<bottom.size(); ++j){
                if(edge_map[i].find(j) == edge_map[i].end()){
                    for(int z=0; z<bottom[i].size(); ++z){
                        for(int w=0; w<bottom[j].size(); ++w){
                            vector<int> edge;
                            edge.push_back(bottom[i][z]->dual_die->initial_index2());
                            edge.push_back(bottom[j][w]->dual_die->initial_index2());
                            edge.push_back(1);
                            edges.push_back(edge);
                            out.insert(j);
                        }
                    }
                }
            }
            for(int j=i+1; j<input_edge_num+i+1; ++j){
                for(int s=i+input_edge_num+1; s<bottom.size(); ++s){
                    if(in.find(j) != in.end() && out.find(s)!=out.end()){
                        for(int z=0; z<bottom[j].size(); ++z){
                            for(int w=0; w<bottom[s].size(); ++w){
                                vector<int> edge;
                                edge.push_back(bottom[j][z]->dual_die->initial_index2());
                                edge.push_back(bottom[s][w]->dual_die->initial_index2());
                                edge.push_back(0);
                                edges.push_back(edge);
                                edge_map[j].insert(s);
                                edge_map[s].insert(j);
                            }
                        }
                    }
                }
            }
        }
    }
    for(int i=0; i<bottom.size(); ++i){
        t_bottom.insert(t_bottom.end(), bottom[i].begin(), bottom[i].end());
    }
    t_bottom.push_back(t_root);
    for(int i=0; i<edges.size(); ++i){
        t_edges.push_back(edges[i]);
    }
}
void GlobalPlacer::m_graph_connection(MST_node* t_root1, MST_node* t_root2, vector<vector<int>>& t_h_edges, vector<vector<int>>& t_v_edges){
    queue<MST_node*> q1;
    queue<MST_node*> q2;
    q1.push(t_root1);
    q2.push(t_root2);
    vector<MST_node*> nodes_1;
    vector<MST_node*> nodes_2;
    MST_node* current_node;
    while(q1.size() > 0){
        current_node = q1.front();
        q1.pop();
        for(int i=0; i<current_node->bottom_nodes.size(); ++i){
            nodes_1.push_back(current_node->bottom_nodes[i]);
            q1.push(current_node->bottom_nodes[i]);
        }
    }
    while(q2.size() > 0){
        current_node = q2.front();
        q2.pop();
        for(int i=0; i<current_node->bottom_nodes.size(); ++i){
            nodes_2.push_back(current_node->bottom_nodes[i]);
            q2.push(current_node->bottom_nodes[i]);
        }
    }
    
    vector<vector<int>> h_edges;
    vector<vector<int>> v_edges;
    int dir = rand()%2;
    int in_graph;
    for(int i=0; i<nodes_1.size(); ++i){
        for(int j=0; j<nodes_2.size(); ++j){
            vector<int> edge;
            if(dir == 0){
                edge.push_back(nodes_1[i]->dual_die->initial_index2());
                edge.push_back(nodes_2[j]->dual_die->initial_index2());
            }
            else{
                edge.push_back(nodes_2[j]->dual_die->initial_index2());
                edge.push_back(nodes_1[i]->dual_die->initial_index2());                
            }
            edge.push_back(1);
            h_edges.push_back(edge);
        }
    }
    for(int i=0; i<h_edges.size(); ++i){
        t_h_edges.push_back(h_edges[i]);
    }
}

void GlobalPlacer::m_initial_topology_generation(ECG* t_ECGs){
    set<die*> h_set;
    set<die*> v_set;
    vector<die*> h_vec;
    vector<die*> v_vec;
    vector<EMIB*> h_edge;
    vector<EMIB*> v_edge;
    vector<EMIB*> r_edge;
    vector<ECG*>  h_ecg;
    vector<ECG*>  v_ecg;
    die* die_1;
    die* die_2;
    die* root;
    set<die*>::iterator it_1;
    set<die*>::iterator it_2;
    int r_edge_num = t_ECGs->EMIBset.size();
    int h_die_num;
    int v_die_num;
    while(r_edge_num > t_ECGs->EMIBset.size()*0.3){
        h_die_num = rand()%t_ECGs->dieset.size();
        v_die_num = t_ECGs->dieset.size() - h_die_num;
        v_set.clear();
        h_set.clear();
        v_edge.clear();
        h_edge.clear();
        r_edge.clear();
        for(int i=0; i<t_ECGs->dieset.size(); ++i){
            int h_in = rand()%2;
            if(h_in == 0){
                if(h_set.size() >= h_die_num){
                    v_set.insert(t_ECGs->dieset[i]);
                }
                else{
                    h_set.insert(t_ECGs->dieset[i]);
                }
            }
            else{
                if(v_set.size() >= v_die_num){
                    h_set.insert(t_ECGs->dieset[i]);
                }
                else{
                    v_set.insert(t_ECGs->dieset[i]);
                }
            }
        }
        for(int i=0; i<t_ECGs->EMIBset.size(); ++i){
            die_1 = t_ECGs->dieset[t_ECGs->EMIBset[i]->die_1];
            die_2 = t_ECGs->dieset[t_ECGs->EMIBset[i]->die_2];
            it_1 = h_set.find(die_1);
            it_2 = h_set.find(die_2);    
            if(it_1==h_set.end() && it_2 == h_set.end()){
                
                v_edge.push_back(t_ECGs->EMIBset[i]);
            }
            else if(it_1 == h_set.end() || it_2 == h_set.end()){
                r_edge.push_back(t_ECGs->EMIBset[i]);
            }
            else{
                h_edge.push_back(t_ECGs->EMIBset[i]);
            }
        }
        r_edge_num = r_edge.size();
    }
    for(auto it=h_set.begin(); it!=h_set.end(); ++it){
        h_vec.push_back((*it));   
    }
    for(auto it=v_set.begin(); it!=v_set.end(); ++it){
        v_vec.push_back((*it));   
    }
    for(int i=0; i<h_vec.size(); ++i){
        h_vec[i]->set_initial_index(i);
    }
    for(int i=0; i<v_vec.size(); ++i){
        v_vec[i]->set_initial_index(i);
    }
    for(int i=0; i<h_edge.size(); ++i){
        EMIB* sub_emib = new EMIB(t_ECGs->dieset[h_edge[i]->die_1]->initial_index(), t_ECGs->dieset[h_edge[i]->die_2]->initial_index(), h_edge[i]->overlap, h_edge[i]->distance, h_edge[i]->occupied);
        h_edge[i] = sub_emib;
    }
    for(int i=0; i<v_edge.size(); ++i){
        EMIB* sub_emib = new EMIB(t_ECGs->dieset[v_edge[i]->die_1]->initial_index(), t_ECGs->dieset[v_edge[i]->die_2]->initial_index(), v_edge[i]->overlap, v_edge[i]->distance, v_edge[i]->occupied);
        v_edge[i] = sub_emib;
    }
    cout << "HCG Node: " << endl;
    for(int i=0; i<h_vec.size(); ++i){
        cout << h_vec[i]->initial_index2() << ",";
    }
    cout << endl;
    cout << "VCG Node: " << endl;
    for(int i=0; i<v_vec.size(); ++i){
        cout << v_vec[i]->initial_index2() << ",";
    }
    cout << endl;
    cout << "HCG EMIB edge num: " << h_edge.size() << endl;
    cout << "VCG EMIB edge num: " << v_edge.size() << endl;
    m_ECG_extraction(h_vec, h_edge, h_ecg);
    m_ECG_extraction(v_vec, v_edge, v_ecg);
    for(int i=0; i<h_edge.size(); ++i){
        delete h_edge[i];
    }
    for(int i=0; i<v_edge.size(); ++i){
        delete v_edge[i];
    }
    cout << "h subgroup num: " << h_ecg.size() << endl;
    cout << "v_subgroup num: " << v_ecg.size() << endl;
    vector<MST_node*>   h_roots;
    vector<MST_node*>   v_roots;
    MST_node            virtual_h_root(0);
    MST_node            virtual_v_root(0);
    vector<MST_node*>   h_abovenode;
    vector<MST_node*>   v_abovenode;
    MST_node* MST_root;
    for(int i=0; i<h_ecg.size(); ++i){
        for(int j=0; j<h_ecg[i]->dieset.size(); ++j){
            h_ecg[i]->dieset[j]->Reset();
            h_ecg[i]->dieset[j]->set_initial_index(j);
        }
        m_maximum_spanning_tree(h_ecg[i]->dieset, h_ecg[i]->EMIBset, MST_root);
        m_reduction_assignment(MST_root, h_abovenode, t_ECGs->h_edges);
        cout << "ww" << endl;
        for(int i=0; i<t_ECGs->h_edges.size(); ++i){
            cout << t_ECGs->h_edges[i][0] << "--->" << t_ECGs->h_edges[i][1] << endl;
        }
        h_roots.push_back(MST_root);
        h_abovenode.clear();
    }
    for(int i=0; i<h_roots.size(); ++i){
        virtual_h_root.bottom_nodes.push_back(h_roots[i]);
    }
    m_random_assignment(&virtual_h_root, h_abovenode, t_ECGs->v_edges);
    for(int i=0; i<v_ecg.size(); ++i){
        for(int j=0; j<v_ecg[i]->dieset.size(); ++j){
            v_ecg[i]->dieset[j]->Reset();
            v_ecg[i]->dieset[j]->set_initial_index(j);
        }
        m_maximum_spanning_tree(v_ecg[i]->dieset, v_ecg[i]->EMIBset, MST_root);
        m_reduction_assignment(MST_root, v_abovenode, t_ECGs->v_edges);
        v_roots.push_back(MST_root);
        v_abovenode.clear();
    }
    for(int i=0; i<v_roots.size(); ++i){
        virtual_v_root.bottom_nodes.push_back(v_roots[i]);
    }
    m_random_assignment(&virtual_v_root, v_abovenode, t_ECGs->h_edges);
    cout << "ss" << endl;
    for(int i=0; i<t_ECGs->h_edges.size(); ++i){
        cout << t_ECGs->h_edges[i][0] << "--->" << t_ECGs->h_edges[i][1] << endl;
    }
    m_graph_connection(&virtual_h_root, &virtual_v_root, t_ECGs->h_edges, t_ECGs->v_edges);
    cout << "VCG edge: " << endl;
    for(int i=0; i<t_ECGs->v_edges.size(); ++i){
        cout << t_ECGs->v_edges[i][0] << "--->" << t_ECGs->v_edges[i][1] << endl;
    }
    cout << "HCG edge: " << endl;
    for(int i=0; i<t_ECGs->h_edges.size(); ++i){
        cout << t_ECGs->h_edges[i][0] << "--->" << t_ECGs->h_edges[i][1] << endl;
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
        " "<< current_die->current_h() << " " << current_die->initial_index2() << endl;
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

vector<pair<float, float>>  GlobalPlacer::m_TransformECGToTCG(){
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
    vector<pair<int, int>> inf;
    for(int i=0; i<m_EMIBNets.size(); ++i){
        inf.push_back(make_pair(m_EMIBNets[i]->die_1, m_EMIBNets[i]->die_2));
    }
    return inf;
}

void GlobalPlacer::m_ECG_extraction(vector<die*>& t_dies, vector<EMIB*>& t_EMIBs, vector<ECG*>&  t_ECGs){
    vector<vector<int>>   ECGset;
    vector<vector<EMIB*>> EMIBset;
    vector<int>::iterator die1_found;
    vector<int>::iterator die2_found;
    int                   die1_in;
    int                   die2_in;
    int                   low;
    int                   high;
    int                   in;
    int                   out;
    int                   local_die1 = -1;
    int                   local_die2 = -1;
    for(int i=0; i<t_EMIBs.size(); ++i){
        die1_in = ECGset.size();
        die2_in = ECGset.size();
        for(int j=0; j<ECGset.size(); ++j){
            if(die1_in == ECGset.size()){
                die1_found = find(ECGset[j].begin(), ECGset[j].end(), t_EMIBs[i]->die_1);
                if(die1_found != ECGset[j].end()){
                    die1_in = j;
                }
            }
            if(die2_in == ECGset.size()){
                die2_found = find(ECGset[j].begin(), ECGset[j].end(), t_EMIBs[i]->die_2);
                if(die2_found != ECGset[j].end()){
                    die2_in = j;
                }
            }
            if(die1_in != ECGset.size() && die2_in != ECGset.size()){
                break;
            }
        }
        if(die1_in != ECGset.size() && die2_in != ECGset.size()){
            if(die1_in != die2_in){
                low = (die1_in < die2_in)?(die1_in):(die2_in);
                high = (die1_in > die2_in)?(die1_in):(die2_in);
                ECGset[low].insert(ECGset[low].end(), ECGset[high].begin(), ECGset[high].end());
                ECGset.erase(ECGset.begin()+high);
                EMIBset[low].insert(EMIBset[low].end(), EMIBset[high].begin(), EMIBset[high].end());
                EMIBset[low].push_back(t_EMIBs[i]);
                EMIBset.erase(EMIBset.begin()+high);
            }
            else{
                EMIBset[die1_in].push_back(t_EMIBs[i]);
            }
        }
        else if(die1_in == ECGset.size() && die2_in == ECGset.size()){
            vector<int> new_set;
            vector<EMIB*> new_EMIBset;
            new_set.push_back(t_EMIBs[i]->die_1);
            new_set.push_back(t_EMIBs[i]->die_2);
            ECGset.push_back(new_set);
            new_EMIBset.push_back(t_EMIBs[i]);
            EMIBset.push_back(new_EMIBset);
        }
        else{
            in = (die1_in == ECGset.size())?(die2_in):(die1_in);
            out = (die1_in == ECGset.size())?(t_EMIBs[i]->die_1):(t_EMIBs[i]->die_2);
            ECGset[in].push_back(out);
            EMIBset[in].push_back(t_EMIBs[i]);
        }
    }
    for(int i=0; i<ECGset.size(); ++i){
        ECG* new_ecg = new ECG;
        for(int j=0; j<ECGset[i].size(); ++j){
            new_ecg->dieset.push_back(t_dies[ECGset[i][j]]);
            t_dies[ECGset[i][j]]->set_backup(1);
        }
        for(int j=0; j<EMIBset[i].size(); ++j){
            local_die1 = -1;
            local_die2 = -1;
            for(int w=0; w<new_ecg->dieset.size(); ++w){
                if(local_die1 == -1){
                    if(t_dies[EMIBset[i][j]->die_1] == new_ecg->dieset[w]){
                        local_die1 = w;
                    }
                }
                if(local_die2 == -1){
                    if(t_dies[EMIBset[i][j]->die_2] == new_ecg->dieset[w]){
                        local_die2 = w;
                    }
                }
                if(local_die1 != -1 && local_die2 != -1){
                    EMIB* new_EMIB = new EMIB(local_die1, local_die2, EMIBset[i][j]->overlap, EMIBset[i][j]->distance, EMIBset[i][j]->occupied);
                    new_ecg->EMIBset.push_back(new_EMIB);
                    break;
                }
            }
        }
        t_ECGs.push_back(new_ecg);
    }
    for(int i=0; i<t_dies.size(); ++i){
        if(!t_dies[i]->backup()){
            ECG* new_ecg = new ECG;
            new_ecg->dieset.push_back(t_dies[i]);
            t_ECGs.push_back(new_ecg);
        }
    }
    for(int i=0; i<t_dies.size(); ++i){
        t_dies[i]->set_backup(0);
    }
}


