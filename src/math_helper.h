#ifndef MATH_HELPER_H
#define MATH_HELPER_H
#include<ostream>
#include<cmath>
#define sqrt2 1.41421356237310f
namespace superpunto{
  //////////////FLOAT3//////////////////////////////////////////////////
  struct float3{
    float x,y,z;
    float3 operator+ (float3 b){ return {x+b.x, y+b.y, z+b.z};}
    void operator+= (float3 b){ x+=b.x; y+=b.y; z+=b.z;}
    float3 operator- (float3 b){ return {x-b.x, y-b.y, z-b.z};}
    void operator-= (float3 b){ x-=b.x; y-=b.y; z-=b.z;}
    float3 operator* (float3 b){ return {x*b.x, y*b.y, z*b.z};}
    float3 operator/ (float3 b){ return {x/b.x, y/b.y, z/b.z};}

    bool   operator< (float b ){ return x<b && y<b && z<b; }

    template <typename T>
    float3 operator* (T b){ return {b*x, b*y, b*z};}
    template <typename T>
    float3 operator/ (T b){ return {x/b, y/b, z/b};}
    template <typename T>
    float3 operator+ (T b){ return {b+x, b+y, b+z};}
    template <typename T>
    float3 operator- (T b){ return {x-b, y-b, z-b};}
  
    float dot(float3 b){return x*b.x+y*b.y+z*b.z;}
    float norm(){ return x*x+y*y+z*z; }
    float max(){ return x>y?(x>z?x:z):(y>z?y:z);}
  };

  std::ostream &operator<<(std::ostream &os, float3 const &a);

  float3 abs(float3 a);
  float3 floor(float3 a);

  float3 make_float3(float x, float y, float z);
  float3 make_float3(float x);

  //////////////FLOAT4//////////////////////////////////////////////////
  struct float4{
    float x,y,z,w;
    float4 operator+ (float4 b){ return {x+b.x, y+b.y, z+b.z, w+b.w};}
    void operator+= (float4 b){ x+=b.x; y+=b.y; z+=b.z; w+=b.w;}
    float4 operator- (float4 b){ return {x-b.x, y-b.y, z-b.z, w-b.w};}
    void operator-= (float4 b){ x-=b.x; y-=b.y; z-=b.z; w-= b.w;}
    float4 operator* (float4 b){ return {x*b.x, y*b.y, z*b.z, w*b.w};}
    float4 operator/ (float4 b){ return {x/b.x, y/b.y, z/b.z, w/b.w};}

    template <typename T>
    float4 operator* (T b){ return {b*x, b*y, b*z, b*w};}
    template <typename T>
    float4 operator/ (T b){ return {x/b, y/b, z/b, w/b};}
    template <typename T>
    float4 operator+ (T b){ return {b+x, b+y, b+z, w+b};}
    template <typename T>
    float4 operator- (T b){ return {x-b, y-b, z-b, w-b};}

    float dot(float4 b){return x*b.x+y*b.y+z*b.z+w*b.w;}
    float norm(){ return x*x+y*y+z*z+w*w; }

  };

  std::ostream &operator<<(std::ostream &os, float4 const &a);

  float4 abs(float4 a);
  float4 floor(float4 a);

  float4 make_float4(float3 a, float w);
  float4 make_float4(float3 a);
  float4 make_float4(float x, float y, float z);
  float4 make_float4(float x);


  //////////////FLOAT2//////////////////////////////////////////////////
  struct float2{
    float x,y;
    float2 operator+ (float2 b){ return {x+b.x, y+b.y};}
    void operator+= (float2 b){ x+=b.x; y+=b.y;}
    float2 operator- (float2 b){ return {x-b.x, y-b.y};}
    void operator-= (float2 b){ x-=b.x; y-=b.y;}
    float2 operator* (float2 b){ return {x*b.x, y*b.y};}
    float2 operator/ (float2 b){ return {x/b.x, y/b.y};}

    bool   operator< (float b ){ return x<b && y<b ; }

    template <typename T>
    float2 operator* (T b){ return {b*x, b*y};}
    template <typename T>
    float2 operator/ (T b){ return {x/b, y/b};}
    template <typename T>
    float2 operator+ (T b){ return {b+x, b+y};}
    template <typename T>
    float2 operator- (T b){ return {x-b, y-b};}

    float dot(float2 b){return x*b.x+y*b.y;}
    float norm(){ return x*x+y*y; }
  };

  std::ostream &operator<<(std::ostream &os, float2 const &a);

  float2 abs(float2 a);
  float2 floor(float2 a);

  float2 make_float2(float x, float y);
  float2 make_float2(float x);
  //////////////COMPLEX//////////////////////////////////////////////////
  struct complex{
    float x,y;
    complex operator+ (complex b){ return {x+b.x, y+b.y};}
    void operator+= (complex b){ x+=b.x; y+=b.y;}
    complex operator- (complex b){ return {x-b.x, y-b.y};}
    void operator-= (complex b){ x-=b.x; y-=b.y;}

    complex operator* (complex b){ return {(x*b.x-y*b.y), (y*b.x + x*b.y)};} //dot
    complex operator/ (complex b){ 
      float r = norm();
      return {(x*b.x+y*b.y)/r, (y*b.x - x*b.y)/r};
    }

    template <typename T>
    complex operator* (T b){ return {b*x, b*y};}
    template <typename T>
    complex operator/ (T b){ return {x/b, y/b};}
    template <typename T>
    void operator/= (T b){ x/=b; y/=b;}
    template <typename T>
    complex operator+ (T b){ return {b+x, b+y};}
    template <typename T>
    complex operator- (T b){ return {x-b, y-b};}

    float norm(){ return x*x-y*y; }
    complex conj(){ return {x, -y};}
    complex inv(){ 
      float r = norm();
      return {x/r, -y/r};
    }
  };

  std::ostream &operator<<(std::ostream &os, complex const &a);
  complex expi(float a);
  complex make_complex(float x, float y);
  complex make_complex(float x);




  //////////////INT3//////////////////////////////////////////////////
  struct int3{
    int x,y,z;
    int3 operator+ (int3 b){ return {x+b.x, y+b.y, z+b.z};}
    void operator+= (int3 b){ x+=b.x; y+=b.y; z+=b.z;}
    int3 operator- (int3 b){ return {x-b.x, y-b.y, z-b.z};}
    void operator-= (int3 b){ x-=b.x; y-=b.y; z-=b.z;}
    int3 operator* (int3 b){ return {x*b.x, y*b.y, z*b.z};}
    int3 operator/ (int3 b){ return {x/b.x, y/b.y, z/b.z};}

    bool   operator< (int b ){ return x<b && y<b && z<b; }

    template <typename T>
    int3 operator* (T b){ return {b*x, b*y, b*z};}
    template <typename T>
    int3 operator/ (T b){ return {x/b, y/b, z/b};}
    template <typename T>
    int3 operator+ (T b){ return {b+x, b+y, b+z};}
    template <typename T>
    int3 operator- (T b){ return {x-b, y-b, z-b};}

    int dot(int3 b){return x*b.x+y*b.y+z*b.z;}
    int norm(){ return x*x+y*y+z*z; }
    int max(){ return x>y?(x>z?x:z):(y>z?y:z);}
    operator float3(){return {(float)x, (float)y, (float)z};}
  };

  std::ostream &operator<<(std::ostream &os, int3 const &a);

  int3 abs(int3 a);

  int3 make_int3(int x, int y, int z);
  int3 make_int3(int x);

  //////////////INT2//////////////////////////////////////////////////
  struct int2{
    int x,y;
    int2 operator+ (int2 b){ return {x+b.x, y+b.y};}
    void operator+= (int2 b){ x+=b.x; y+=b.y;}
    int2 operator- (int2 b){ return {x-b.x, y-b.y};}
    void operator-= (int2 b){ x-=b.x; y-=b.y;}
    int2 operator* (int2 b){ return {x*b.x, y*b.y};}
    int2 operator/ (int2 b){ return {x/b.x, y/b.y};}

    bool   operator< (int b ){ return x<b && y<b ; }

    template <typename T>
    int2 operator* (T b){ return {b*x, b*y};}
    template <typename T>
    int2 operator/ (T b){ return {x/b, y/b};}
    template <typename T>
    int2 operator+ (T b){ return {b+x, b+y};}
    template <typename T>
    int2 operator- (T b){ return {x-b, y-b};}

    int dot(int2 b){return x*b.x+y*b.y;}
    int norm(){ return x*x+y*y; }
  };

  std::ostream &operator<<(std::ostream &os, int2 const &a);

  int2 abs(int2 a);

  int2 make_int2(int x, int y);
  int2 make_int2(int x);


}
#endif
