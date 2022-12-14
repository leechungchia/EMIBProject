#include "sa.h"
#include "globalplacer.h"

////            floorplan start             ////

////set random  seed and base arguments////

void SA::set_random_seed(int t_seed){
    m_seed = t_seed;
}

void SA::set_SA_parameter(float t_initial_temperature, float t_decay_rate, float t_epsilon, unsigned  t_k, unsigned t_time_upperbound){
    m_initial_temperature = t_initial_temperature;
    m_decay_rate = t_decay_rate;
    m_epsilon = t_epsilon;
    m_k = t_k;
    m_time_upperbound = t_time_upperbound;
}


void SA::set_profile_parameter(float t_bound_arg, float t_area_arg, float t_hpwl_arg){
    m_bound_arg = t_bound_arg;
    m_area_arg = t_area_arg;
    m_hpwl_arg = t_hpwl_arg;
}

void SA::set_boundary_parameter(float t_x_bound, float t_y_bound){
    m_x_bound = t_x_bound;
    m_y_bound = t_y_bound;
}

float SA::get_profile_cost(){
    float x_max = m_BstarTree->max_x();
    float y_max = m_BstarTree->max_y();
    float x_distance = x_max-m_x_bound;
    float y_distance = y_max-m_y_bound;
    float bound_x_cost = (m_bound_arg*x_distance/m_x_bound>=0)?m_bound_arg*x_distance/m_x_bound:0;
    float bound_y_cost = (m_bound_arg*y_distance/m_y_bound>=0)?m_bound_arg*y_distance/m_y_bound:0;
    float bound_cost = bound_x_cost + bound_y_cost;
    return bound_cost;
}

void  SA::set_bstar_parameter(float t_swap_p, float t_di_p, float t_rotate_p){
    if((t_swap_p + t_di_p + t_rotate_p == 1)&&(t_swap_p>=0)&&(t_di_p>=0)&&(t_rotate_p>=0)){
        m_swap_p = t_swap_p;
        m_di_p = t_di_p;
        m_rotate_p = t_rotate_p;
    }
    else{
        cout << "wrong input of move probability" << endl;
    }
}

float SA::get_area_cost(){
    if(m_structure == "B*-tree"){
        return m_area_arg*m_BstarTree->area()/m_area_base;
    }
    return 0;
}

float SA::get_hpwl_cost(){
    if(m_structure == "B*-tree"){
        return m_area_arg*m_BstarTree->HPWL()/m_hpwl_base;
    }
    return 0;
}

////get the gain////

float SA::get_current_cost(int t_mode, vector<float> t_input)
{
    float current_cost=1000;
    float profile_cost;
    float area_cost;
    float hpwl_cost;
    switch(t_mode){
        case 1:
            current_cost = t_input[0]*m_reduction_para + t_input[1]*m_illegal_para;
            break;
        case 2:
            if(t_input[2] >= 0){
                current_cost = t_input[0]*m_x_para + t_input[1]*m_y_para + t_input[2]*m_delta_para;
            }
            else{
                current_cost = t_input[0]*m_x_para + t_input[1]*m_y_para + t_input[2]*9999;
            }
            break;
        case 4:
            current_cost = t_input[0]*m_x_para + t_input[1]*m_y_para;
            break;
        case 3:
            profile_cost = get_profile_cost();
            area_cost    = get_area_cost();
            hpwl_cost    = get_hpwl_cost();
            current_cost = profile_cost + area_cost + hpwl_cost;
            break;
        default:
            current_cost = 1000;
            break;
    }

    return current_cost;
}

float SA::get_current_best_cost()
{
    return m_best_cost;
}

////get the probability////

bool SA::m_chosen_probability(float diff, float current_temperature)
{
    m_calltime++;
    if(diff < 0)
    {
        return true;
    }
    else
    {
        float check = (float)(rand()%10)/(float)10;
        return check < (float)exp(-diff/current_temperature);
    }
}



////copy the structure of the bstar tree////


////do the sa move////

void SA::m_bstar_move()
{
    m_calltime ++;
    m_BstarTree->do_backups();
    float move_case = (float)(rand()%10)/(float)10.0;
    if(move_case < m_di_p)
    {
        bool dim = rand()%2;
        int index_1 = rand()%m_size;
        int index_2 = rand()%m_size;
        while(index_2 == index_1)
        {
            index_2 = rand()%m_size;
        }
        m_BstarTree->delete_insert(m_BstarTree->nodes()->at(index_1), m_BstarTree->nodes()->at(index_2), dim);
    }
    else if(move_case < m_swap_p + m_di_p)
    {
        int index_1 = rand()%m_size;
        int index_2 = rand()%m_size;
        while(index_2 == index_1)
        {
            index_2 = rand()%m_size;
        }
        m_BstarTree->swap_move(m_BstarTree->nodes()->at(index_1), m_BstarTree->nodes()->at(index_2));
    }
    else if(move_case < m_swap_p + m_di_p + m_transfer_p){
        int index = rand()%m_BstarTree->m_ecgs.size();
        bool change = ((float)(rand()%10)/(float)10.0 < m_change_group_p);
        float change_group = m_BstarTree->m_ecgs[index]->current_num.first;
        bool dir = rand()%2;
        int step = 1+rand()%3;
        if(change){
            change_group = rand()%m_BstarTree->m_ecgs[index]->topology_set.size();
        }
        cout << m_BstarTree->m_ecgs[index]->topology_set.size() << endl;
        cout << m_BstarTree->m_ecgs[index]->current_num.first << endl;
        cout << index << "," << change_group << "," << dir << "," << step << endl;
        m_BstarTree->transfer_topology(m_BstarTree->m_ecgs[index], change_group, dir, step);
    }
    else
    {
        int index = rand()%m_size;
        m_BstarTree->rotate(m_BstarTree->nodes()->at(index));
    }
    cout << "mm" << endl;
    m_BstarTree->m_get_coordinates();
}

////progress of simulated annealing////

void SA::start(){
    m_bstar_start();
}
void SA::m_bstar_start()
{
    float current_temperature = m_initial_temperature;
    int reject = 0;
    int training_time = 1;
    int uphill = 0;
    float current_cost = 0;
    float last_cost = m_best_cost;
    float diff = 0;
    long t_start = clock();
    long time_process = 0;
    long ticks = 1000000.0;
    while(((float)reject/(float)training_time < 0.95)&&(current_temperature >= m_epsilon))
    {
        training_time = 1;
        uphill = 0;
        reject = 0;
        while((uphill <= m_time_upperbound)&&(training_time <= 2*m_time_upperbound))
        {
            training_time += 1;
            cout << "uu" << endl;
            m_bstar_move();
            cout << "cc" << endl;
            vector<float> temp;
            current_cost = get_current_cost(3, temp);
            diff = current_cost - last_cost;
            if(m_chosen_probability(diff, current_temperature))
            {
                if(diff > 0)
                {
                    uphill += 1;
                }
                if(current_cost < m_best_cost)
                {
                    m_BstarTree->restore_bests();
                    m_best_cost = current_cost;
                    cout << "current cost: " << m_best_cost << endl;
                    cout << "bound: " << get_profile_cost() << "area: " << get_area_cost() << "hpwl: " << get_hpwl_cost() << endl; 
                }
                last_cost = current_cost;
            }
            else
            {
                m_BstarTree->get_backups();
                reject += 1;
            }
        }
        current_temperature *= m_decay_rate;
		long t_end = clock();
		time_process = t_end - t_start;
		if(time_process> ticks*60*10){
			break;
		}
    }
    m_BstarTree->get_bests();
}

vector<vector<float>> SA::get_dies_inf(int t_index_1, int t_index_2){
    vector<vector<float>> result;
    if(m_structure == "B*-tree"){
        for(int i=0; i<m_size; ++i){
            result.push_back(m_BstarTree->get_dies_coor(i));
        }
    }
    else if(m_structure == "TCG"){
        for(int i=0; i<m_size; ++i){
            result.push_back(m_TCGs[t_index_1][t_index_2]->get_dies_coor(i));
        }
    }

    return result;
}

void SA::phase1_start(){
    float current_temperature = m_initial_temperature;
    int reject = 0;
    int training_time = 1;
    int uphill = 0;
    float current_cost = 10000;
    float best_cost    = 10000;
    float last_cost    = 10000;
    float diff = 0;
    /*long t_start = clock();*/
    long time_process = 0;
    long ticks = 1000000.0;
    int move;
    int die1=0;
    int die2=0;
    bool legalize_success = false;
    bool back = false;
    bool have_new_group = false;
    bool repeated = false;
    int repeated_solutions = 0;
    int total_solution = 0;
    bool has_solution = false;
    bool find_solution = false;
    bool time_out = false;
    TCG* new_TCG;
    TCG* current_TCG = base_TCG->copyself(false);
    TCG* last_TCG    = base_TCG->copyself(false);
    vector<pair<float, float>> DieVec; 
    vector<vector<float>> EMIBNetVec; 
    vector<pair<int, int>> MappingEMIBToDie;
    vector<vector<int>> h_edges;
    vector<vector<int>> v_edges;
    cout << "start phase 1" << endl;
    while((((float)repeated_solutions/(float)(m_buffer+total_solution) < 0.95) || (!has_solution)) && (!time_out) && (m_TCGs.size() < m_base_num))
    {
        if(back){
            has_solution = true;
            total_solution++;
            have_new_group = true;
            repeated = false;
            current_cost = 10000;
            best_cost    = 10000;
            diff = 0;
            for(int i=0; i<m_TCGs.size(); ++i){
                if(placer->m_check_similarity(m_TCGs[i][0], current_TCG) > m_similarity_bound){
                    if(placer->m_check_equal(current_TCG, m_TCGs[i])){
                        repeated_solutions++;
                        repeated = true;
                        have_new_group = false;
                        break;
                    }
                    new_TCG = current_TCG->copyself(true);
                    new_TCG->BackUp();
                    m_TCGs[i].push_back(new_TCG);
                    have_new_group = false;
                    m_topology_num++;
                    break;
                }
            }
            if(have_new_group){
                new_TCG = current_TCG->copyself(true);
                new_TCG->BackUp();
                vector<TCG*> new_group = {new_TCG};
                m_TCGs.push_back(new_group);
                m_topology_num++;
            }
            placer->m_initial_topology_generation(ECG_index);
            DieVec = placer->m_ECGs[ECG_index]->TransformDieToTCG();
            EMIBNetVec = placer->m_ECGs[ECG_index]->TransformEMIBToTCG();
            MappingEMIBToDie = placer->m_ECGs[ECG_index]->MappingEMIBToDie();
            h_edges.clear();
            v_edges.clear();
            h_edges = placer->m_ECGs[ECG_index]->h_edges;
            v_edges = placer->m_ECGs[ECG_index]->v_edges;
            delete base_TCG;
            base_TCG = new TCG();
            base_TCG->TCGConstruct(DieVec, EMIBNetVec, MappingEMIBToDie);
            base_TCG->GetTCGEdge(h_edges, v_edges);
            delete current_TCG;
            delete last_TCG;
            current_TCG = base_TCG->copyself(false);
            last_TCG    = base_TCG->copyself(false);
            current_temperature = m_initial_temperature;
        }
        back = false;
        reject = 0;
        training_time = 1;
        while(((float)reject/(float)training_time < 0.95)&&(current_temperature >= m_epsilon)){
            if(back){
                break;
            }
            uphill = 0;
            while((uphill <= m_time_upperbound)&&(training_time <= 2*m_time_upperbound))
            {
                training_time += 1;
                move = rand()%4;
                die1 = rand()%current_TCG->m_TCGNodes.size();
                die2 = rand()%current_TCG->m_TCGNodes.size();
                while(die2 == die1){
                    die2 = rand()%current_TCG->m_TCGNodes.size();
                }
                switch(move){
                    case 0:
                        current_TCG->movement_rotation(die1);
                        break;
                    case 1:
                        current_TCG->movement_swap(die1, die2);
                        break;
                    case 2:
                        current_TCG->movement_reverse(die1);
                        break;
                    case 3:
                        current_TCG->movement_move(die1);
                        break;
                    default:
                        break;
                }
                legalize_success =current_TCG->Legalization();
                vector<float> tcg_in = {current_TCG->ReductionRatio(), current_TCG->LegalNodeRatio()};
                current_cost = get_current_cost(1, tcg_in);
                diff = current_cost - last_cost;
                if(legalize_success){
                    back = true;
                    break;
                }
                if(m_chosen_probability(diff, current_temperature))
                {
                    if(diff > 0)
                    {
                        uphill += 1;
                    }
                    last_cost = current_cost;
                    delete last_TCG;
                    last_TCG = current_TCG;
                    current_TCG = current_TCG->copyself(false);
                }
                else
                {
                    delete current_TCG;
                    current_TCG = last_TCG->copyself(false);
                    reject += 1;
                }
            }
            current_temperature *= m_decay_rate;
            /*long t_end = clock();
            time_process = t_end - t_start;
            if(time_process > ticks*60*1){
                time_out = true;
            }*/
        }

    }
}
void SA::phase2_start(){
    float current_temperature = m_initial_temperature_2;
    int reject = 0;
    int training_time = 1;
    int uphill = 0;
    float current_cost = 10000;
    float best_cost    = 10000;
    float last_cost    = 10000;
    float diff = 0;
    /*long t_start = clock();*/
    long time_process = 0;
    long ticks = 1000000.0;
    int move;
    int die1;
    int die2;
    bool legalize_success = false;
    bool have_new_group = false;
    bool repeated = false;
    int repeated_solutions = 0;
    int total_solution = 0;
    int iteration = 0;
    bool has_solution = false;
    bool find_solution = false;
    TCG* new_TCG;
    TCG* last_TCG;
    TCG* current_TCG;
    TCG* base;
    TCG* inserted;
    cout << "start phase 2" << endl;
    for(int i=0; i<m_TCGs.size(); ++i){
        base        = m_TCGs[i][0];
        iteration = 0;
        current_TCG = m_TCGs[i][0]->copyself(false);
        last_TCG    = m_TCGs[i][0]->copyself(false);
        current_temperature = m_initial_temperature_2;
        repeated_solutions = 0;
        total_solution     = 0;
        reject = 0;
        training_time = 1;
        while((repeated_solutions/(m_buffer_2+total_solution) < 0.95) && ((float)reject/(float)training_time < 0.95)&&(current_temperature > m_epsilon))
        {
            uphill = 0;
            while((uphill <= m_time_upperbound)&&(training_time <= 2*m_time_upperbound))
            {
                training_time += 1;
                move = rand()%4;
                die1 = rand()%current_TCG->m_TCGNodes.size();
                die2 = rand()%current_TCG->m_TCGNodes.size();
                while(die2 == die1){
                    die2 = rand()%current_TCG->m_TCGNodes.size();
                }
                switch(move){
                    case 0:
                        current_TCG->movement_rotation(die1);
                        break;
                    case 1:
                        current_TCG->movement_swap(die1, die2);
                        break;
                    case 2:
                        current_TCG->movement_reverse(die1);
                        break;
                    case 3:
                        current_TCG->movement_move(die1);
                        break;
                    default:
                        break;
                }
                legalize_success =current_TCG->Legalization();
                if(legalize_success){
                    vector<float> tcg_in;
                    tcg_in.push_back((float)current_TCG->m_HCG->target()->value()/base->m_HCG->target()->value());
                    tcg_in.push_back((float)current_TCG->m_VCG->target()->value()/base->m_VCG->target()->value());
                    tcg_in.push_back((float)(placer->m_check_similarity(current_TCG, base) - m_similarity_bound_2)/(float)(1.0-m_similarity_bound_2));
                    current_cost = get_current_cost(2, tcg_in);
                    diff = current_cost - last_cost;
                    if(!placer->m_check_equal(current_TCG, m_TCGs[i])){
                        inserted = current_TCG->copyself(true);
                        inserted->BackUp();
                        m_TCGs[i].push_back(inserted);
                    }
                    else{
                        repeated_solutions++;
                    }
                    total_solution++;
                }
                if((legalize_success) && m_chosen_probability(diff, current_temperature))
                {
                    if(diff > 0)
                    {
                        uphill += 1;
                    }
                    last_cost = current_cost;
                    delete last_TCG;
                    last_TCG = current_TCG;
                    current_TCG = current_TCG->copyself(false);
                }
                else
                {
                    delete current_TCG;
                    current_TCG = last_TCG->copyself(false);
                    reject += 1;
                }
            }
            current_temperature *= m_decay_rate;
            /*long t_end = clock();
            time_process = t_end - t_start;*/
        }
    }
    set<TCG*> temp;
    for(int i=0; i<m_TCGs.size(); ++i){
        temp.insert(m_TCGs[i].begin(), m_TCGs[i].end());
    }
    cout << "equal ratio: " << equal_ratio(temp) << endl;
}


float SA::equal_ratio(set<TCG*> t_seq){
    int counter = 0;
    int size    = t_seq.size();
    bool exist = false;
    while(t_seq.size() > 0){
        exist = false;
        TCG* current = (*t_seq.begin());
        for(auto it=t_seq.begin(); it!=t_seq.end(); ++it){
            if((*it) != current && placer->m_check_similarity((*it), current) == 1.0){
                t_seq.erase(it++);
                exist = true;
            }
        }
        if(exist){
            counter++;
        }
        t_seq.erase(t_seq.begin());
    }
    return float(counter)/(float)size;
}



