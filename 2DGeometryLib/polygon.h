#pragma once
#include <vector>
#include <utility>
#include <ostream>

#define GEOMETRYLIBRARY_API __declspec(dllexport)

namespace geometry2d
{
	struct GEOMETRYLIBRARY_API Point
	{
		double x;
		double y;

		Point(double _x, double _y) : x{ _x }, y{ _y } {}

		Point() : Point(0, 0) {}

		Point(std::pair<double, double> point) : Point(point.first, point.second) {}
	};

	GEOMETRYLIBRARY_API std::ostream& operator<<(std::ostream& os, const Point& point);

	class Polygon
	{
		std::vector<Point> points;
		double _area;

		//The leftmost and lowest point of boundary box
		Point min;
		//The rightmost and highest point of boundary box
		Point max;

		void computeArea();

	public:
		GEOMETRYLIBRARY_API Polygon(const std::vector<Point>& collection);

		//Polygon area computed using Gauss's area formula with O(n) complexity
		//Current version works for non self-intersecting polygons
		//improving it via Bentley-Ottmann Algorithm to find all intersections will cost additional
		//O((n+k)log(n)) complexity where n - number of sides and k - number of intersections
		//Possible solution: creating polygon interface with virtual area() and two derived classes:
		//SelfIntersectingPolygon and usual Polygon.
		GEOMETRYLIBRARY_API double area() const;

		//Implemented using ray tracing algorithm with O(n) complexity
		//Returns false for points on polygon sides
		GEOMETRYLIBRARY_API bool isPointInside(const Point& point) const;

		GEOMETRYLIBRARY_API const std::vector<Point>& getPoints() const
		{
			return points;
		}
	};

	enum class GEOMETRYLIBRARY_API TurnType { Left, Right, Collinear };

	//Returns turn to recognize 
	GEOMETRYLIBRARY_API TurnType getTurnType(const Point& a, const Point& b, const Point& c);

	//Graham-Andrew scan with O(n*4log(n)) complexity
	GEOMETRYLIBRARY_API Polygon buildConvexHull(std::vector<Point> points);

	GEOMETRYLIBRARY_API bool isPointOnLine(const Point& point, const std::pair<Point, Point>& line);
}