#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"
#include "Point.h"
#include <memory>

class Rectangle:public Shape{
public:
	Rectangle(const Point&p1,const Point&p2);

	double getArea()constoverride;
	Point getCenter()constoverride;
	void move(double dx,double dy)override;
	void scale(double factor)override;
	std::string getName()constoverride;
	std::unique_ptr<Shape>clone()constoverride;

private:
	Point bottomLeft_;
	Point topRight_;
};

#endif
