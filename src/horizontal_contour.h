#ifndef HORIZONTAL_CONTOUR_H
#define HORIZONTAL_CONTOUR_H

#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>
#include <cfloat>
#include <iterator>

using namespace std;

class segment;
class horizontal_contour;

class segment
{
public:
    segment(){}
    float    s_xcoor();
    float    l_xcoor();
    float    value();
    segment* fore_link();
    segment* rear_link();
    void     set_segment(float t_sxcoor, float t_lxcoor, float t_value, segment* t_flink, segment* t_rlink);
    void     set_fore_link(segment* t_segment);
    void     set_rear_link(segment* t_segment);
    void     set_value(float t_value);
    void     set_sxcoor(float t_sxcoor);
    void     set_lxcoor(float t_lxcoor);
private:
    float m_s_xcoor;
    float m_l_xcoor;
    float m_value;
    segment* m_flink;
    segment* m_rlink;
};

class horizontal_contour
{
public:
    horizontal_contour()
    {
        m_max_length = FLT_MAX;
        m_min_length = FLT_MIN;
        m_min_value  = 0;
        m_height = m_min_value;
        segment* ground = new segment();
        segment* ceiling = new segment();
        m_ground = ground;
        m_ceiling = ceiling;
        segment* intermediate = new segment();
        intermediate->set_segment(m_min_length, m_max_length, m_min_value, m_ground, m_ceiling);
        m_ground->set_rear_link(intermediate);
        m_ceiling->set_fore_link(intermediate);
    }
    float insert_segment(float s_xcoor, float l_xcoor, float t_height);
    void  present();
    void  reset();
    segment* ground(){return m_ground;}
    segment* ceiling(){return m_ceiling;}
private:
    segment* m_ground;
    segment* m_ceiling;
    float    m_max_length;
    float    m_min_length;
    float    m_min_value;
    float    m_height;
};

#endif

