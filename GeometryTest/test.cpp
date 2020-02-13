#include "pch.h"
#include "polygon.h"

TEST(AreaTest, SimpleSquare)
{
	std::vector<geometry2d::Point> p{ {0, 0}, {1, 0}, {1, 1}, {0, 1} };
	geometry2d::Polygon square{ p };
	EXPECT_DOUBLE_EQ(square.area(), 1.0);
}

TEST(AreaTest, SimpleRectangle)
{
	geometry2d::Polygon rectangle{ {{-1, -1}, {-1, 3}, {1, 3}, {1, -1}} };
	EXPECT_DOUBLE_EQ(rectangle.area(), 8.0);
}

TEST(AreaTest, ConvexPentagon)
{
	geometry2d::Polygon polygon{ {{4,2}, {10,6}, {8,8}, {3,12}, {2,7} } };
	EXPECT_DOUBLE_EQ(polygon.area(), 40.5);
}

TEST(AreaTest, NonConvexPentagon)
{
	geometry2d::Polygon polygon{ {{4.5,8}, {10,6}, {8,8}, {3,12}, {2,7} } };
	EXPECT_DOUBLE_EQ(polygon.area(), 16.25);
}

TEST(AreaTest, ManyPointsRectangle)
{
	geometry2d::Polygon square4p{ {{-10, -10}, {-10, 10}, {10, 10}, {10, -10}} };
	EXPECT_DOUBLE_EQ(square4p.area(), 400.0);
	
	std::vector<geometry2d::Point> points;
	for (int i = -10; i < 10; i++)
	{
		points.emplace_back(i, -10);
	}
	for (int i = -10; i < 10; i++)
	{
		points.emplace_back(10, i);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(i, 10);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(-10, i);
	}

	geometry2d::Polygon square{ points };
	EXPECT_EQ(square.getPoints().size(), 4);
	EXPECT_EQ(square.area(), 400);
}

TEST(PointsOnSideOptimization, Bad0Point)
{
	std::vector<geometry2d::Point> points;
	for (int i = -9; i < 10; i++)
	{
		points.emplace_back(i, -10);
	}
	for (int i = -10; i < 10; i++)
	{
		points.emplace_back(10, i);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(i, 10);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(-10, i);
	}
	points.emplace_back(-10, -10);

	geometry2d::Polygon square{ points };
	EXPECT_EQ(square.getPoints().size(), 4);
	EXPECT_EQ(square.area(), 400);
}

TEST(PointOnLine, PointOnLine)
{
	EXPECT_FALSE(geometry2d::isPointOnLine({ -0.5, 1 }, { {-1, 0}, {-1, 2} }));
	EXPECT_TRUE(geometry2d::isPointOnLine({ -1, 1 }, { {-1, 0}, {-1, 2} }));
	EXPECT_TRUE(geometry2d::isPointOnLine({ -1, 0 }, { {-1, 0}, {-1, 2} }));
	EXPECT_FALSE(geometry2d::isPointOnLine({ -1, -1 }, { {-1, 0}, {-1, 2} }));
}

TEST(PointInsideTest, SimpleSquare)
{
	geometry2d::Polygon square{ {{0, 0}, {1, 0}, {1, 1}, {0, 1}} };
	EXPECT_FALSE(square.isPointInside({ -1, 1 }));
	EXPECT_TRUE(square.isPointInside({ 0.5, 0.5 }));
	EXPECT_TRUE(square.isPointInside({ 0.3, 0.7 }));
	EXPECT_FALSE(square.isPointInside({ 0.5, 1 }));
}

TEST(PointInsideTest, SelfIntersectingPolygon)
{
	geometry2d::Polygon polygon{ {{-1, -1}, {1, 4.5}, {3, 0}, {3, 3}, {-1, 3.5}, {8, 4} }};
	EXPECT_FALSE(polygon.isPointInside({ 0, 3 }));
	EXPECT_FALSE(polygon.isPointInside({ 0, 4 }));
	EXPECT_FALSE(polygon.isPointInside({ 1, 3.5 }));
	EXPECT_FALSE(polygon.isPointInside({ 2.5, 2.5 }));
	EXPECT_TRUE(polygon.isPointInside({ 1, 2 }));
	EXPECT_TRUE(polygon.isPointInside({ 0.5, 3.5 }));
}

TEST(PointInsideTest, ConvexPentagon)
{
	geometry2d::Polygon polygon{ {{4,2}, {10,6}, {8,8}, {3,12}, {2,7} } };
	EXPECT_TRUE(polygon.isPointInside({ 7, 8 }));
	EXPECT_TRUE(polygon.isPointInside({ 9, 6.5 }));
	EXPECT_TRUE(polygon.isPointInside({ 5, 7 }));
	EXPECT_FALSE(polygon.isPointInside({ 2, 11 }));
	EXPECT_FALSE(polygon.isPointInside({ 3, 4 }));
}

TEST(PointInsideTest, NonConvexPentagon)
{
	geometry2d::Polygon polygon{ {{4.5,8}, {10,6}, {8,8}, {3,12}, {2,7} } };
	EXPECT_TRUE(polygon.isPointInside({ 7, 8 }));
	EXPECT_TRUE(polygon.isPointInside({ 9, 6.5 }));
	EXPECT_FALSE(polygon.isPointInside({ 5, 7 }));
	EXPECT_FALSE(polygon.isPointInside({ 2, 11 }));
}

TEST(TurnTest, LeftTurn)
{
	EXPECT_EQ(geometry2d::getTurnType({ 0, 0 }, { 2, 0 }, { 0.5, 1 }), geometry2d::TurnType::Left);
}

TEST(TurnTest, RightTurn)
{
	EXPECT_EQ(geometry2d::getTurnType({ 0, 0 }, { 0, 1 }, { 0.5, 0.5 }), geometry2d::TurnType::Right);
}

TEST(TurnTest, CollineareCase)
{
	EXPECT_EQ(geometry2d::getTurnType({ 0, 0 }, { 0, 1 }, { 0, 2 }), geometry2d::TurnType::Collinear);
}

TEST(ConvexHull, Case1)
{
	geometry2d::Polygon hullPoly = geometry2d::buildConvexHull({ {4,2}, {5,8}, {7,6}, {3,8}, {8,6}, {5, 10}, {10,6}, {8,8}, {3,12}, {2,7} });
	std::vector<geometry2d::Point> expectedHull{ {4,2}, {10,6}, {8,8}, {3,12}, {2,7} };
	auto hull = hullPoly.getPoints();
	ASSERT_EQ(hull.size(), expectedHull.size());
	for (size_t i = 0; i < hull.size(); i++)
	{
		EXPECT_DOUBLE_EQ(expectedHull[hull.size() - i - 1].x, hull[i].x);
		EXPECT_DOUBLE_EQ(expectedHull[hull.size() - i - 1].y, hull[i].y);
	}
}

TEST(ConvexHull, Case2)
{
	std::vector<geometry2d::Point> points;
	for (int i = -10; i < 10; i++)
	{
		points.emplace_back(i, -10);
	}
	for (int i = -10; i < 10; i++)
	{
		points.emplace_back(10, i);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(i, 10);
	}
	for (int i = 10; i > -10; i--)
	{
		points.emplace_back(-10, i);
	}
	
	geometry2d::Polygon hullPoly = geometry2d::buildConvexHull(points);
	std::vector<geometry2d::Point> expectedHull{ {-10, -10}, {-10, 10}, {10, 10}, {10, -10} };
	auto hull = hullPoly.getPoints();
	ASSERT_EQ(hull.size(), expectedHull.size());
	for (size_t i = 0; i < hull.size(); i++)
	{
		EXPECT_DOUBLE_EQ(expectedHull[i].x, hull[i].x);
		EXPECT_DOUBLE_EQ(expectedHull[i].y, hull[i].y);
	}
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}