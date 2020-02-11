#include "pch.h"
#include "polygon.h"
#include <algorithm>
#include <stdexcept>


std::ostream& geometry2d::operator<<(std::ostream& os, const Point& point)
{
	os << "{" << point.x << "," << point.y << "}";
	return os;
}


void geometry2d::Polygon::computeArea()
{
	_area = 0.0;

	for (size_t i = 0; i < points.size() - 1; i++)
	{
		_area += (points[i].x + points[i + 1].x) * (points[i].y - points[i + 1].y);
	}

	_area += (points[points.size() - 1].x + points[0].x) * (points[points.size() - 1].y - points[0].y);
	_area = 0.5 * abs(_area);
}

geometry2d::Polygon::Polygon(const std::vector<Point>& collection) : points{ collection }
{
	if (collection.empty())
	{
		throw std::invalid_argument("empty point collection is not allowed");
	}

	min = collection[0];
	max = collection[0];
	for (size_t i = 1; i < collection.size(); i++)
	{
		max.x = std::max(max.x, collection[i].x);
		max.y = std::max(max.y, collection[i].y);

		min.x = std::min(min.x, collection[i].x);
		min.y = std::min(min.y, collection[i].y);
	}

	computeArea();
}

double geometry2d::Polygon::area() const
{
	return _area;
}

bool isRayIntersecting(const geometry2d::Point& point, std::pair<geometry2d::Point, geometry2d::Point> side)
{
	//check if point between y coordinates of side
	if (!(side.first.y > point.y) ^ (side.second.y > point.y))
	{
		return false;
	}

	//find x coordinate of intersection point
	double intersectionPointX = (side.second.x - side.first.x) * (point.y - side.first.y) / (side.second.y - side.first.y) + side.first.x;

	//it should be larger because ray directed from left to right 
	return point.x < intersectionPointX;
}

bool geometry2d::Polygon::isPointInside(const Point& point) const
{
	//check if point is outside of polygon bounding rectangle
	if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y)
	{
		return false;
	}

	//if ray crosses sides even number of times, point is outside of polygon
	bool isIntersecting = false;
	for (size_t currentVert = 0, prevVert = points.size() - 1; currentVert < points.size(); prevVert = currentVert++)
	{
		if (isRayIntersecting(point, { points[currentVert], points[prevVert] }))
		{
			isIntersecting = !isIntersecting;
		}
	}

	return isIntersecting;
}

geometry2d::TurnType geometry2d::getTurnType(const geometry2d::Point &a, const geometry2d::Point &b, const geometry2d::Point &c)
{
	double det = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
	if (det > std::numeric_limits<double>::epsilon())
	{
		return TurnType::Left;
	}
	if (det < -std::numeric_limits<double>::epsilon())
	{
		return TurnType::Right;
	}
	return TurnType::Collinear;
}

geometry2d::Polygon geometry2d::buildConvexHull(std::vector<Point> points)
{
	if (points.size() <= 2)
	{
		return Polygon{ points };
	}

	//sort by x coordinate
	std::sort(points.begin(), points.end(), [](const Point& a, const Point& b)
	{
		return a.x < b.x || a.x == b.x && a.y < b.y;
	});
	
	const Point firstPoint = points.front(),
		lastPoint = points.back();

	//we will have two separated parts of the hull
	std::vector<Point> rightPart{ firstPoint },
		leftPart{ firstPoint };

	//loop behavior is depended on fact, where new point lies regarding the line
	//between the leftmost and the rightmost points
	for (size_t i = 1; i < points.size(); ++i)
	{
		if (i == points.size() - 1 || getTurnType(firstPoint, points[i], lastPoint) == TurnType::Right)
		{
			while (rightPart.size() >= 2 && getTurnType(rightPart[rightPart.size() - 2], rightPart[rightPart.size() - 1], points[i]) != TurnType::Right)
			{
				rightPart.pop_back();
			}
			rightPart.push_back(points[i]);
		}
		else if (i == points.size() - 1 || getTurnType(firstPoint, points[i], lastPoint) == TurnType::Left)
		{
			while (leftPart.size() >= 2 && getTurnType(leftPart[leftPart.size() - 2], leftPart[leftPart.size() - 1], points[i]) != TurnType::Left)
			{
				leftPart.pop_back();
			}
			leftPart.push_back(points[i]);
		}
	}

	//build hull from parts
	points.clear();
	for (size_t i = 0; i < rightPart.size(); ++i)
	{
		points.push_back(rightPart[i]);
	}
	for (size_t i = leftPart.size() - 2; i > 0; --i)
	{
		points.push_back(leftPart[i]);
	}
	return Polygon{ points };
}

bool geometry2d::isPointOnLine(const Point& point, const std::pair<Point, Point>& line)
{
	double dxPoint = point.x - line.first.x;
	double dyPoint = point.y - line.first.y;

	double dxLine = line.second.x - line.first.x;
	double dyLine = line.second.y - line.first.y;

	double cross = dxPoint * dyLine - dyPoint * dxLine;

	if (cross != 0)
	{
		return false;
	}

	if (abs(dxLine) >= abs(dyLine))
	{
		return dxLine > 0 ?
			line.first.x <= point.x && point.x <= line.second.x :
			line.second.x <= point.x && point.x <= line.first.x;
	}

	return dyLine > 0 ?
		line.first.y <= point.y && point.y <= line.second.y :
		line.second.y <= point.y && point.y <= line.first.y;
}