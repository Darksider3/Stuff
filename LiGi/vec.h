#ifndef VEC_H
#define VEC_H

typedef struct
{
  double x;
  double y;
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
};

#endif // VEC_H
