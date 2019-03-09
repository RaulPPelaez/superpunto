#include"math_helper.h"
namespace superpunto{
  using std::abs;
  using std::floor;

  //////////////FLOAT3//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, float3 const &a){
    return os<<a.x<<" "<<a.y<<" "<<a.z;
  }
  float3 abs(float3 a){  return {abs(a.x), abs(a.y), abs(a.z)};}
  float3 floor(float3 a){ return {floor(a.x), floor(a.y), floor(a.z)};}
  float3 make_float3(float x, float y, float z){ return {x,y,z};}
  float3 make_float3(float x){ return {x,x,x};}
  //////////////FLOAT4//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, float4 const &a){
    return os<<a.x<<" "<<a.y<<" "<<a.z;
  }

  float4 abs(float4 a){  return {abs(a.x), abs(a.y), abs(a.z), abs(a.w)};}
  float4 floor(float4 a){ return {floor(a.x), floor(a.y), floor(a.z), floor(a.w)};}
  float4 make_float4(float x, float y, float z, float w){ return {x,y,z,w};}
  float4 make_float4(float3 a, float w){ return {a.x,a.y,a.z,w};}
  float4 make_float4(float3 a){ return {a.x,a.y,a.z,0};}
  float4 make_float4(float x){ return {x,x,x,x};}
  //////////////FLOAT2//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, float2 const &a){
    return os<<a.x<<" "<<a.y;
  }
  float2 abs(float2 a){  return {abs(a.x), abs(a.y)};}
  float2 floor(float2 a){ return {floor(a.x), floor(a.y)};}
  float2 make_float2(float x, float y){ return {x,y};}
  float2 make_float2(float x){ return {x,x};}
  //////////////COMPLEX//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, complex const &a){
    return os<<a.x<<" "<<a.y;
  }
  complex expi(float a){
    return {std::cos(a), std::sin(a)};
  }
  complex make_complex(float x, float y){ return {x,y};}
  complex make_complex(float x){ return {x,x};}

  //////////////INT3//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, int3 const &a){
    return os<<a.x<<" "<<a.y<<" "<<a.z;
  }
  int3 abs(int3 a){ return {abs(a.x), abs(a.y), abs(a.z)};}
  int3 make_int3(int x, int y, int z){ return {x,y,z};}
  int3 make_int3(int x){return {x, x, x};}
  //////////////INT2//////////////////////////////////////////////////
  std::ostream &operator<<(std::ostream &os, int2 const &a){
    return os<<a.x<<" "<<a.y;
  }
  int2 abs(int2 a){  return {abs(a.x), abs(a.y)};}
  int2 make_int2(int x, int y){ return {x,y};}
  int2 make_int2(int x){ return {x,x};}


}
