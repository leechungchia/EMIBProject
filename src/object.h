#ifndef OBJECT_H
#define OBJECT_H

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
#include "string"

using namespace std;


class rectangle{
    public:
        rectangle(string t_code_name, float t_width, float t_height):
        m_code_name(t_code_name), m_width(t_width), m_height(t_height), m_rotated(0), m_backup(0){};
        rectangle(){}
        float h(){return m_height;};
        float w(){return m_width;};
        float current_w(){
            if(m_rotated == 0 || m_rotated == 2){
                return m_width;
            }
            else{
                return m_height;
            }
        }
        float current_h(){
            if(m_rotated == 0 || m_rotated == 2){
                return m_height;
            }
            else{
                return m_width;
            }
        }
        float x(){return m_xcoor;};
        float y(){return m_ycoor;};
        float l_x(){
            if(m_rotated == 0 || m_rotated == 2){
                return m_xcoor+m_width;
            }
            else{
                return m_xcoor+m_height;
            }
        }
        float l_y(){
            if(m_rotated == 0 || m_rotated == 2){
                return m_ycoor+m_height;
            }
            else{
                return m_ycoor+m_width;
            }
        }
        void set_initial_index(int t_index){m_init_index = t_index;};
        int  initial_index(){return m_init_index;};
        void set_initial_index2(int t_index){m_init_index_2 = t_index;};
        int  initial_index2(){return m_init_index_2;};
        int  r(){return m_rotated;};
        string code_name(){return m_code_name;};
        void set_h(float t_height){m_height = t_height;};
        void set_w(float t_width){m_width = t_width;};
        void set_coor(float t_xcoor, float t_ycoor){m_xcoor = t_xcoor; m_ycoor=t_ycoor;}
        void set_x(float t_xcoor){m_xcoor = t_xcoor;}
        void set_y(float t_ycoor){m_ycoor = t_ycoor;}
        void set_code_name(string t_code_name){m_code_name = t_code_name;};
        void set_r(int t_rotated){m_rotated = t_rotated;};
        void set_backup(bool t_back){m_backup = t_back;};
        bool backup(){return m_backup;};
    private:
        string m_code_name;
        float  m_width;
        float  m_height;
        float  m_xcoor;
        float  m_ycoor;
        int    m_rotated;
        int    m_init_index;
        int    m_init_index_2;
        bool    m_backup;
};

class pin
{
public:
    pin(string t_code_name, float t_width_diff, float t_height_diff):
    m_code_name(t_code_name), m_w_diff(t_width_diff), m_h_diff(t_height_diff){}
    string code_name(){return m_code_name;};
    float  pin_xcoor(float t_xcoor, float t_width, float t_height, int t_rotated){
        if(t_rotated == 0){
            return t_xcoor+m_w_diff;
        }
        else if(t_rotated == 1){
            return t_xcoor+t_height-m_h_diff;
        }
        else if(t_rotated == 2){
            return t_xcoor+t_width-m_w_diff;
        }
        else{
            return t_xcoor+m_h_diff;
        }
    }
    float pin_ycoor(float t_ycoor, float t_width, float t_height, int t_rotated){
        if(t_rotated == 0){
            return t_ycoor+m_h_diff;
        }
        else if(t_rotated == 1){
            return t_ycoor+m_w_diff;
        }
        else if(t_rotated == 2){
            return t_ycoor+t_height-m_h_diff;
        }
        else{
            return t_ycoor+t_width-m_w_diff;
        }
    }
    float              w_diff(){return m_w_diff;};
    float              h_diff(){return m_h_diff;};
private:
    string             m_code_name;
    float              m_w_diff;
    float              m_h_diff;
};


#endif