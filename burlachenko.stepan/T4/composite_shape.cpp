#include "composite_shape.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

void CompositeShape::addShape(const std::shared_ptr<Shape>& shape)
{
    if (shape == nullptr)
    {
        throw std::invalid_argument("Shape pointer is null");
    }

    shapes_.push_back(shape);
}

double CompositeShape::getArea() const
{
    double totalArea = 0.0;

    for (const std::shared_ptr<Shape>& shape : shapes_)
    {
        totalArea += shape->getArea();
    }

    return totalArea;
}

Point CompositeShape::getCenter() const
{
    if (shapes_.empty())
    {
        throw std::invalid_argument("Composite shape is empty");
    }

    Point firstCenter = shapes_[0]->getCenter();
    double minX = firstCenter.x;
    double maxX = firstCenter.x;
    double minY = firstCenter.y;
    double maxY = firstCenter.y;

    for (const std::shared_ptr<Shape>& shape : shapes_)
    {
        Point currentCenter = shape->getCenter();

        if (currentCenter.x < minX)
        {
            minX = currentCenter.x;
        }

        if (currentCenter.x > maxX)
        {
            maxX = currentCenter.x;
        }

        if (currentCenter.y < minY)
        {
            minY = currentCenter.y;
        }

        if (currentCenter.y > maxY)
        {
            maxY = currentCenter.y;
        }
    }

    Point center;
    center.x = (minX + maxX) / 2.0;
    center.y = (minY + maxY) / 2.0;

    return center;
}

void CompositeShape::move(const Point& newCenter)
{
    if (shapes_.empty())
    {
        throw std::invalid_argument("Composite shape is empty");
    }

    Point oldCenter = getCenter();

    double dx = newCenter.x - oldCenter.x;
    double dy = newCenter.y - oldCenter.y;

    for (const std::shared_ptr<Shape>& shape : shapes_)
    {
        Point currentCenter = shape->getCenter();
        Point movedCenter;

        movedCenter.x = currentCenter.x + dx;
        movedCenter.y = currentCenter.y + dy;

        shape->move(movedCenter);
    }
}

void CompositeShape::scale(double ratio)
{
    if (ratio <= 0.0)
    {
        throw std::invalid_argument("Ratio must be positive");
    }

    if (shapes_.empty())
    {
        throw std::invalid_argument("Composite shape is empty");
    }

    Point compositeCenter = getCenter();

    for (const std::shared_ptr<Shape>& shape : shapes_)
    {
        Point shapeCenter = shape->getCenter();
        Point newCenter;

        newCenter.x = compositeCenter.x + (shapeCenter.x - compositeCenter.x) * ratio;
        newCenter.y = compositeCenter.y + (shapeCenter.y - compositeCenter.y) * ratio;

        shape->move(newCenter);
        shape->scale(ratio);
    }
}

std::string CompositeShape::getName() const
{
    return "COMPOSITE";
}

std::string CompositeShape::getDescription() const
{
    std::ostringstream oss;
    Point center = getCenter();
    oss << std::fixed << std::setprecision(2);
    oss << "[COMPOSITE, (" << center.x << ", " << center.y << "), " << getArea() << ":\n";
    
    for (size_t i = 0; i < shapes_.size(); i++)
    {
        Point c = shapes_[i]->getCenter();
        oss << "  " << shapes_[i]->getName()
            << ", (" << c.x << ", " << c.y << "), " << shapes_[i]->getArea();

        if (i + 1 < shapes_.size())
        {
            oss << ",";
        }

        oss << "\n";
    }
    oss << "]";
    return oss.str();
}
