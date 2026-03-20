#include "Ring.h"
#include "Point.h"
#include <cmath>
#include <stdexcept>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ring::Ring(const Point& center, double outRad, double inRad)
    : center_(center), outR_(outRad), inR_(inRad) {

    if (outR_ <= 0) {
        throw std::invalid_argument("Error: Outer radius must be positive.");
    }
    if (inR_ < 0) {
        throw std::invalid_argument("Error: Inner radius cannot be negative.");
    }
    if (inR_ >= outR_) {
        throw std::invalid_argument("Error: Inner radius must be less than outer radius.");
    }
}

double Ring::getArea() const {
    return M_PI * (outR_ * outR_ - inR_ * inR_);
}

Point Ring::getCenter() const {
    return center_;
}

void Ring::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Ring::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Error: Scale factor must be positive.");
    }
    outR_ *= factor;
    inR_ *= factor;
}

std::string Ring::getName() const {
    return "RING";
}
