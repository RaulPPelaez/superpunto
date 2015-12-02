#ifndef HELPER_H
#define HELPER_H

#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include<string>
#include<sstream>   
#include<vector>
#include<cmath>
#include<defines.h>
using namespace std;
/*
#define fori(x,y) for(int i=x; i<y;i++)
#define forj(x,y) for(int j=x; j<y;j++)
#define fork(x,y) for(int k=x; k<y;k++)
#define forl(x,y) for(int l=x; l<y;l++)

#define RANDESP ((double)rand()/(double)RAND_MAX) 
#define PI2 3.1415*2.0
*/
double pow(double x, int n);
double dist(const vector<double> &a, const vector<double> &b);
double square_dist(const vector<double> &a, const vector<double> &b);
double square_mod(const vector<double> &a);
void rand_vec(int l, double a, double b, vector<double> &res);
template<typename T>
void vsub(const vector<T> &a, const vector<T> &b, vector<T> &c){
  fori(0,a.size())c[i] = b[i]-a[i];
 
}

double vmul(const vector<double> &a, const vector<double> &b);
void get_inbounds(vector<double> &a);



class REvent{
 public:
  //REvent();
  enum EventType{
    box_resized,
    N_changed
  };
  EventType type;
};


#endif
