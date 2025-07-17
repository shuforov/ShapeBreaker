#include "../include/Vec2.h"
#include <cmath>
#include <math.h>

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
  return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const { return Vec2(x / val, y / val); }

Vec2 Vec2::operator*(const float val) const { return Vec2(x * val, y * val); }

bool Vec2::operator==(const Vec2 &rhs) const {
  if (x == rhs.x && y == rhs.y) {
    return true;
  }
  return false;
}

bool Vec2::operator!=(const Vec2 &rhs) const {
  if (x != rhs.x || y != rhs.y) {
    return true;
  }
  return false;
}

void Vec2::operator+=(const Vec2 &rhs) {
  x += rhs.x;
  y += rhs.y;
}

void Vec2::operator-=(const Vec2 &rhs) {
  x -= rhs.x;
  y -= rhs.y;
}

void Vec2::operator*=(const float val) {
  x *= val;
  y *= val;
}

void Vec2::operator/=(const float val) {
  x /= val;
  y /= val;
}

float Vec2::dist(const Vec2 &rhs) const {
  float dx = rhs.x - x;
  float dy = rhs.y - y;
  return std::sqrt((dx * dx) + (dy * dy));
}

Vec2 Vec2::normalizeToTarget(const Vec2 &target) const {
  Vec2 differenceVector = Vec2(target.x - x, target.y - y);
  float length = std::sqrt((differenceVector.x * differenceVector.x) +
                           (differenceVector.y * differenceVector.y));
  return Vec2(differenceVector.x / length, differenceVector.y / length);
}
