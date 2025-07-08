#include "../include/Vec2.h"
#include <math.h>

Vec2::Vec2() {
}


Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

Vec2 Vec2::operator + (const Vec2& rhs) const {
  // TODO
  return Vec2(0, 0);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
  // TODO
  return Vec2(0,0);
}

Vec2 Vec2::operator / (const float val) const {
  // TODO
  return Vec2(0, 0);
}

Vec2 Vec2::operator * (const float val) const {
  // TODO
  return Vec2(0, 0);
}

bool Vec2::operator == (const Vec2& rhs) const {
  // TODO
  return false;
}

bool Vec2::operator != (const Vec2& rhs) const {
  // TODO
  return false;
}

void Vec2::operator += (const Vec2& rhs) {
  // TODO
}

void Vec2::operator -= (const Vec2& rhs) {
  // TODO
}

void Vec2::operator *= (const float val) {
  // TODO
}

void Vec2::operator /= (const float val) {
  // TODO
}

float Vec2::dist(const Vec2& rhs) const {
  // TODO
  return 0;
}
