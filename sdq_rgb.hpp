#ifndef SDQ_RGB_HPP
#define SDQ_RGB_HPP

// Squared distance between two RGB values (using 32-bit ARGB types)
inline DATA32 rgb_dist_sqr(DATA32 x, DATA32 y)
{
  const auto rx = (x>>16)&0xff; const auto ry = (y>>16)&0xff;
  const auto gx = (x>>8 )&0xff; const auto gy = (y>>8 )&0xff;
  const auto bx =  x     &0xff; const auto by =  y     &0xff;
  const auto rd = (rx>ry) ? rx-ry : ry-rx;
  const auto gd = (gx>gy) ? gx-gy : gy-gx;
  const auto bd = (bx>by) ? bx-by : by-bx;
  
  return rd*rd + gd*gd + bd*bd;
}

inline bool rgb_dist_lte(DATA32 x, DATA32 y, DATA32 tol)
{
  return rgb_dist_sqr(x,y) <= (tol*tol);
}

#endif // SDQ_RGB_HPP
