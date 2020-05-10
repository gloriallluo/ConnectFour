/*******************************************************
*	Point.h : 棋盘点类                                 	*
*	张永锋                                              *
*	zhangyf07@gmail.com                                *
*	2014.5                                             *
********************************************************/

# ifndef POINT_H_
# define POINT_H_
# include <iostream>

class Point{
public:
	int x;
	int y;
	Point() {}
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}
	Point(const Point &A) {
		x = A.x; y = A.y;
	}
	Point& operator=(const Point &A) {
		x = A.x; y = A.y;
		return *this;
	}
	bool operator==(const Point &A) {
		return x == A.x && y == A.y;
	}
};

#endif
