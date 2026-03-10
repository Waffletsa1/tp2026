#include "recttrapezoid.h"

RectTrapezoid::RectTrapezoid(const Point& bl, double tlen, double blen, double h) : bLeft(bl), topLenght(tlen), bottomLenght(blen), height(h) {}

double RectTrapezoid::getArea() const
{
	return ((topLenght + bottomLenght) / 2) * height;
}

std::string RectTrapezoid::getName() const
{
	return "RECTANGULAR TRAPEZOID";
}

void RectTrapezoid::move(double dx, double dy)
{
	bLeft.x += dx;
	bLeft.y += dy;
}

Point RectTrapezoid::getCenter() const
{
	return Point(bLeft.x + ((topLenght + bottomLenght) / 4), bLeft.y + height / 2);
}

std::pair<Point, Point> RectTrapezoid::Bounds() const
{
	Point lb = bLeft;
	Point tr = Point(bLeft.x + std::max(topLenght, bottomLenght), bLeft.y + height);
	return std::make_pair(lb, tr);
}

void RectTrapezoid::scale(double factor)
{
	Point center = getCenter();
	bLeft.x = center.x - (center.x - bLeft.x) * factor;
	bLeft.y = center.y - (center.y - bLeft.y) * factor;
	height = height * factor;
	topLenght = topLenght * factor;
	bottomLenght = bottomLenght * factor;
}

void RectTrapezoid::print(std::ostream& os) const
{
	os << std::fixed << std::setprecision(2);
	os << "[" << getName() << ", " << getCenter() << ", " << getArea() << "]";
}
