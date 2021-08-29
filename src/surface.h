#include <stdio.h>
#include <vector>
#include <algorithm>

#include <cmath>

#include "util.h"

inline float pow2(float a)
{
  return a * a;
}

struct point
{
  float x, y, z;

  static float distance(const point& a, const point& b)
  {
    return (float)sqrt(pow2(a.x - b.x) + pow2(a.y - b.y) + pow2(a.z - b.z));
  }

  bool eq_xy(const point &a)
  {
    if (float_eq(this->x, a.x) && float_eq(this->y, a.y)) 
      return true;

    return false;
  }
};

struct triangle
{
  point p1, p2, p3;
  
  static triangle gen_triangle_xy_ccw(point a, point b, point c) // Makes the points clockwise ingoring Z
  {
    triangle gen;

    // Compute the determinant of the matrix:
    // a_x a_y 1 a_x a_y 
    // b_x b_y 1 b_x b_y 
    // c_x c_y 1 c_x c_y 
    float det = a.x * b.y + a.y * c.x + b.x * c.y -
                b.y * c.x - a.x * c.y - a.y * b.x;

    if (det > 0.0f) // Just keep the points that way
    {
      gen.p1 = a;
      gen.p2 = b;
      gen.p3 = c;
    }
    else
    {
      gen.p1 = b;
      gen.p2 = a;
      gen.p3 = c;
    }

    return gen;
  }

  // TODO: make better
  bool has_common_vertex_xy(point a, point b, point c)
  {
    if (this->p1.eq_xy(a) || this->p1.eq_xy(b) || this->p1.eq_xy(c) || 
        this->p2.eq_xy(a) || this->p2.eq_xy(b) || this->p2.eq_xy(c) || 
        this->p3.eq_xy(a) || this->p3.eq_xy(b) || this->p3.eq_xy(c))
        return true;
    return false;

  }

  bool point_inside_circumcircle(point p)
  {
    // Calculate a crazy determinant.
    // p1.x-p.x p1.y-p.y pow2(p1.x-p.x)+pow2(p1.y-p.y)
    // p2.x-p.x p2.y-p.y pow2(p2.x-p.x)+pow2(p2.y-p.y)
    // p3.x-p.x p3.y-p.y pow2(p3.x-p.x)+pow2(p3.y-p.y)
    
    const float a = p1.x-p.x; const float b = p1.y-p.y; const float c = pow2(p1.x-p.x)+pow2(p1.y-p.y);
    const float d = p2.x-p.x; const float e = p2.y-p.y; const float f = pow2(p2.x-p.x)+pow2(p2.y-p.y);
    const float g = p3.x-p.x; const float h = p3.y-p.y; const float i = pow2(p3.x-p.x)+pow2(p3.y-p.y);
    
    const float det = a * e * i + b * f * g + c * d * h -
                      c * e * g - a * f * h - b * d * i;

    if (det > 0) return true;
    return false;
  }
};

struct edge
{
  point p1, p2;
  bool valid;

  static bool triangle_has_edge(const triangle &a, const edge &b)
  {
    edge ret;

    edge e = {a.p1, a.p2};
    if (e == b) return true;

    e = {a.p2, a.p3};
    if (e == b) return true;
    
    e = {a.p1, a.p3};
    if (e == b) return true;

    return false;
  }

  bool operator ==(const edge &a)
  {
    if ((this->p1.eq_xy(a.p1) && this->p2.eq_xy(a.p2)) ||
        (this->p2.eq_xy(a.p1) && this->p1.eq_xy(a.p2)))
          return true;

    return false;
  }
};

// Boyer-Watson Delaunay triangulation algorithm.
void delaunay_triangulation(const std::vector<point> &points, std::vector<triangle> &triangulation)
{
  // Begin with super triangle
  point s_a = {-1000.0f, -500.0f, 0.0f};
  point s_b = {0.0f, 500.0f, 0.0f};
  point s_c = {1000.0f, -500.0f, 0.0f};
  
  triangulation.push_back(triangle::gen_triangle_xy_ccw(s_a, s_b, s_c));

  for (const point &p: points)
  {
    std::vector<int> bad_triangles;
    for (int i = 0 ; i < triangulation.size() ; i++)
    {
      if (triangulation[i].point_inside_circumcircle(p)) //Point inside the circumference
      {
        bad_triangles.push_back(i);
      }
    }

    std::vector<edge> polygon;
    // Check if the triangle shares edges with any other.
    // I know it's slow, ok?
    for (int i = 0 ; i < bad_triangles.size() ; i++)  // for triangle in bad triangles
    {
      bool e1 = true;
      bool e2 = true;
      bool e3 = true;

      const triangle &t1 = triangulation[bad_triangles[i]];
      
      for (int j = 0 ; j < bad_triangles.size() ; j++) // edge in this triangle 
      {
        const triangle &t2 = triangulation[bad_triangles[j]];

        // Skip if it's the same triangle
        if (i == j) continue;

        edge a;

        if (e1)
        {
          a = {t1.p1, t1.p2}; // TODO: really????
          if (edge::triangle_has_edge(t2, a))
          {
            e1 = false;
          }
        }

        if (e2)
        {
          a = {t1.p2, t1.p3};
          if (edge::triangle_has_edge(t2, a))
          {
            e2 = false;
          }
        }

        if (e3)
        {
          a = {t1.p1, t1.p3};
          if (edge::triangle_has_edge(t2, a))
          {
            e3 = false;
          }
        }
      }

      // Append non shared edges to the polygon
      if (e1) polygon.push_back({t1.p1, t1.p2}); 
      if (e2) polygon.push_back({t1.p2, t1.p3});
      if (e3) polygon.push_back({t1.p1, t1.p3});
    }

    // Ugly, probably, but it's in reverse order
    std::sort(bad_triangles.begin(), bad_triangles.end(), [](int a, int b) {
          return a > b;
        });

    for (int b_t : bad_triangles)
    {
      triangulation.erase(triangulation.begin() + b_t);
    }

    for (edge e : polygon)
    {
      triangulation.push_back(triangle::gen_triangle_xy_ccw(e.p1, e.p2, p));
    }

  }
   for (const triangle &a : triangulation)
   {
      printf("%f %f %f - %f %f %f - %f %f %f\n", a.p1.x, a.p1.y, a.p1.z, a.p2.x, a.p2.y, a.p2.z, a.p3.x, a.p3.y, a.p3.z);
   }

   // Remove all triangles that share a vertex with the original supertriangle
   for (std::vector<triangle>::iterator it = triangulation.begin();
        it != triangulation.end(); ++it)
    {
      if (it->has_common_vertex_xy(s_a, s_b, s_c))
      {
        triangulation.erase(it);
      }
    }
}

triangle gen_triangle(const point& pivot, const std::vector<point>& point_set)
{
  triangle tri;
  
  // Search for the 2 closest points
  float dist = INFINITY;
  point closest_point, sec_closest_point;

  // First one
  for (const point& a : point_set)
  {
    //printf("a: %f %f %f\n", a.x, a.y, a.z);

    // Check if we're not on the pivot
    if (float_eq(a.x, pivot.x) &&
        float_eq(a.y, pivot.y) &&
        float_eq(a.z, pivot.z)) continue;

    const float d = point::distance(a, pivot);

    if (d < dist)
    {
      dist = d;
      closest_point = a;
    }
  }

  dist = INFINITY;

  // Second one
  for (const point& a : point_set)
  {
    // Check if we're not on the pivot or the closest point
    if ((float_eq(a.x, pivot.x) &&
        float_eq(a.y, pivot.y) &&
        float_eq(a.z, pivot.z)) ||
        (float_eq(a.x, closest_point.x) &&
        float_eq(a.y, closest_point.y) &&
        float_eq(a.z, closest_point.z))) continue;

    const float d = point::distance(a, pivot);
    //printf("Dist: %f\n", d);

    if (d < dist)
    {
      dist = d;
      sec_closest_point = a;
    }
  }

  tri.p1 = pivot;
  tri.p2 = closest_point;
  tri.p3 = sec_closest_point;

  printf("Generated triangle: %f %f %f - %f %f %f - %f %f %f\n", pivot.x,
          pivot.y,
          pivot.z,
          closest_point.x,
          closest_point.y,
          closest_point.z,
          sec_closest_point.x,
          sec_closest_point.y,
          sec_closest_point.z);

  return tri;
}


