#include "sa.h"

////            floorplan start             ////

////set random  seed and base arguments////

void SA::set_random_seed(int t_seed){
    m_seed = t_seed;
    srand(m_seed);
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

float SA::get_current_cost()
{
    float profile_cost = get_profile_cost();
    float area_cost    = get_area_cost();
    float hpwl_cost    = get_hpwl_cost();
    float current_cost = profile_cost + area_cost + hpwl_cost;
    return current_cost;
}

float SA::get_current_best_cost()
{
    return m_best_cost;
}

////get the probability////

bool SA::m_chosen_probability(float diff, float current_temperature)
{
    if(diff <= 0)
    {
        return true;
    }
    else
    {
        float check = (float)(rand()%10)/(float)10;
        return check <= exp(-diff/current_temperature);
    }
}



////copy the structure of the bstar tree////


////do the sa move////

void SA::m_bstar_move()
{
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
    else
    {
        int index = rand()%m_size;
        m_BstarTree->rotate(m_BstarTree->nodes()->at(index));
    }
    m_BstarTree->m_get_coordinates();
}

////progress of simulated annealing////

void SA::start(){
    if(m_structure == "B*-tree"){
        m_bstar_start();
    }
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
            m_bstar_move();
            current_cost = get_current_cost();
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

vector<vector<float>> SA::get_dies_inf(){
    vector<vector<float>> result;
    if(m_structure == "B*-tree"){
        for(int i=0; i<m_size; ++i){
            result.push_back(m_BstarTree->get_dies_coor(i));
        }
    }
    else if(m_structure == "TCG"){
        for(int i=0; i<m_size; ++i){
            result.push_back(m_TCG->get_dies_coor(i));
        }
    }

    return result;
}


