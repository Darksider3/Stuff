#ifndef VEC_H
#define VEC_H
#include <cmath>

typedef struct Vec2d
{
  double x;
  double y;


  Vec2d operator+(Vec2d const &rhs)
  {
    Vec2d res;
    res.x = this->x + rhs.x;
    res.y = this->y + rhs.y;

    return res;
  }

  Vec2d operator+=(Vec2d const &rhs)
  {
    *this = *this + rhs;

    return *this;
  }
} Vec2d;


typedef struct
{
  double x;
  double y;
  double z;
} Vec3d;

class VecTools
{
public:
  VecTools();
  static void Intersect();
  static bool Equals();
  static void Add();
  static void Substract();
  virtual ~VecTools();
};


class Vec2dTools: public VecTools
{
public:
  static void Intersect(const Vec2d* e0, const Vec2d* e1, const Vec2d* e2, const Vec2d* e3, Vec2d* intersectionPoint)
  {
    Vec2d b1, b2;
    b1.x = e1->x - e0->x;
    b1.y = e1->y - e0->y;
    b2.x = e3->x - e2->x;
    b2.y = e3->y - e2->y;

    float t;
    t = (b2.x * (e0->y - e2->y) - b2.y * (e0->x - e2->x)) /
        (-b2.x * b1.y + b1.x * b2.y);

    intersectionPoint->x = e0->x + (t * b1.x);
    intersectionPoint->y = e0->y + (t * b1.y);
  }

  static bool Equals(const Vec2d *l, const Vec2d *r)
  {
    return l->x == r->x && l->y == r->y;
  }

  static void Add(const Vec2d *p0, const Vec2d *p1, Vec2d *result)
  {
    result->x = p0->x + p1->x;
    result->y = p0->y + p1->y;
  }

  static void Substract(const Vec2d *p0, const Vec2d *p1, Vec2d *result)
  {
    result->x = p1->x - p0->x;
    result->y = p1->y - p0->y;
  }

  static double Magnitude(const Vec2d *p)
  {
    return std::sqrt((std::pow(p->x, 2) + std::pow(p->y, 2)));
  }
};

#endif // VEC_H
