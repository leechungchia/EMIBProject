#include "horizontal_contour.h"

////            segment starts          ////

////set the smallest, largest x coordinate and y value////

float segment::s_xcoor()
{
    return m_s_xcoor;
}

void segment::set_sxcoor(float t_sxcoor)
{
    m_s_xcoor = t_sxcoor;
}

void segment::set_lxcoor(float t_lxcoor)
{
    m_l_xcoor = t_lxcoor;
}

float segment::l_xcoor()
{
    return m_l_xcoor;
}

float segment::value()
{
    return m_value;
}

void  segment::set_value(float t_value)
{
    m_value = t_value;
}

segment* segment::fore_link()
{
    return m_flink;
}

segment* segment::rear_link()
{
    return m_rlink;
}

void segment::set_segment(float t_sxcoor, float t_lxcoor, float t_value, segment* t_flink, segment* t_rlink)
{
    m_s_xcoor = t_sxcoor;
    m_l_xcoor = t_lxcoor;
    m_value = t_value;
    m_flink = t_flink;
    m_rlink = t_rlink;
}

void segment::set_fore_link(segment* t_segment)
{
    m_flink = t_segment;
}

void segment::set_rear_link(segment* t_segment)
{
    m_rlink = t_segment;
}


////            segment ends            ////

////            horizontal contour update           ////


void horizontal_contour::find_segment(float s_xcoor, float l_xcoor, vector<pair<float, float>>& t_set){
    segment* tar = m_ground->rear_link();
    segment* begin = m_ground;
    segment* end = m_ceiling;
    float small, large, l_value;
    while(tar != m_ceiling)
    {   
        small = tar->s_xcoor();
        large = tar->l_xcoor();
        if((small <= s_xcoor)&&(large > s_xcoor))
        {
            begin = tar;
            t_set.push_back(make_pair(large, tar->value()));
            if(large >= l_xcoor)
            {
                return;
            }
            else
            {
                tar = tar->rear_link();
            }
        }
        else if((small < l_xcoor)&&(large >= l_xcoor))
        {
            t_set.push_back(make_pair(large, tar->value()));
            return;
        }
        else
        {
            tar = tar->rear_link();  
        }
    }
    t_set.push_back(make_pair(100000000, 0));
}

void horizontal_contour::insert_segment(float s_xcoor, float l_xcoor, float t_height, pair<float, float>& t_ypair)
{
    segment* tar = m_ground->rear_link();
    segment* begin = m_ground;
    segment* end = m_ceiling;
    float small, large, l_value;
    l_value = 0;
    while(tar != m_ceiling)
    {   
        small = tar->s_xcoor();
        large = tar->l_xcoor();
        if(begin != m_ground)
        {
            if(tar->value() > l_value)
            {
                l_value = tar->value();
            }
        }
        if((small <= s_xcoor)&&(large > s_xcoor))
        {
            begin = tar;
            l_value = begin->value();
            if(large >= l_xcoor)
            {
                end = tar;
                tar = m_ceiling;

            }
            else
            {
                tar = tar->rear_link();
            }
        }
        else if((small < l_xcoor)&&(large >= l_xcoor))
        {
            end = tar;
            tar = m_ceiling;
        }
        else
        {
            tar = tar->rear_link();
            
        }
    }
    segment* new_seg = new segment();
    if(begin != end)
    {
        tar = begin->rear_link();
        while(tar != end)
        {
            segment* deleted_tar = tar;
            tar = tar->rear_link();
            delete deleted_tar;

        }
        if(begin != m_ground)
        {
            if(begin->s_xcoor() < s_xcoor)
            {
                begin->set_lxcoor(s_xcoor);
            }
            else
            {
                segment* temp = begin;
                begin = begin->fore_link();
                delete temp;
            }
        }
        if(end != m_ceiling)
        {
            if(end->l_xcoor() > l_xcoor)
            {
                end->set_sxcoor(l_xcoor);
            }
            else
            {
                segment* temp = end;
                end = end->rear_link();
                delete temp;
            }
        }
        begin->set_rear_link(new_seg);
        end->set_fore_link(new_seg);
    }
    else
    {
        small = begin->s_xcoor();
        large = begin->l_xcoor();
        if((small == s_xcoor) && (large == l_xcoor))
        {
            begin->set_value(l_value + t_height);
        }
        else if((small != s_xcoor) && (large == l_xcoor))
        {
            begin->set_lxcoor(s_xcoor);
            end = begin->rear_link();
            begin->set_rear_link(new_seg);
            end->set_fore_link(new_seg);
        }
        else if((small == s_xcoor) && (large != l_xcoor))
        {
            end->set_sxcoor(l_xcoor);
            begin = end->fore_link();
            end->set_fore_link(new_seg);
            begin->set_rear_link(new_seg);
        }
        else
        {
            begin->set_lxcoor(s_xcoor);
            segment* new_seg_1 = new segment();
            new_seg_1->set_segment(l_xcoor, large, begin->value(), new_seg, end->rear_link());
            end = new_seg_1;
            begin->set_rear_link(new_seg);
        }
    }
    new_seg->set_segment(s_xcoor, l_xcoor, l_value+t_height, begin, end);
    if(l_value+t_height > m_height)
    {
        m_height = l_value+t_height;
    }
    t_ypair.first = l_value;
    t_ypair.second = l_value+t_height;
}

////present the horizontal list////

void horizontal_contour::present()
{
    segment* tar = m_ground->rear_link(); 
    cout << "ground of horizontal list" << endl;
    while(tar != m_ceiling)
    {
        cout << tar->s_xcoor() << "---" << tar->l_xcoor() << ": " << tar->value() << endl;
        tar = tar->rear_link();
    }
    cout << "ceiling of horizontal list" << endl;
}

////reset the horizontal contour////

void horizontal_contour::reset()
{
    segment* tar = m_ground->rear_link();
    segment* temp;
    while(tar != m_ceiling)
    {
        temp = tar;
        tar = tar->rear_link();
        delete temp;
    }
    m_max_length = FLT_MAX;
    m_min_length = FLT_MIN;
    m_min_value  = 0;
    m_height = m_min_value;
    segment* intermediate = new segment();
    intermediate->set_segment(m_min_length, m_max_length, m_min_value, m_ground, m_ceiling);
    m_ground->set_rear_link(intermediate);
    m_ceiling->set_fore_link(intermediate);
}

////            horizontal list ends            ////

