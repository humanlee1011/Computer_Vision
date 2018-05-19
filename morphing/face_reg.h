#ifndef face_reg_h
#define face_reg_h

#include <iostream>
#include <vector>
#include "CImg.h"
#include <Eigen/Dense>

#include "./delaunay/vector2.h"
#include "./delaunay/triangle.h"
#include "./delaunay/delaunay.h"

#define Point Vector2<float>
#define Triangle Triangle<float>

using namespace cimg_library;
using namespace std;
using namespace Eigen;

// struct Point {
// 	int x, y;
// 	Point(int _x, int _y) {
// 		x = _x;
// 		y = _y;
// 	}
// 	Point() {
// 		x = y = 0;
// 	}
// };

// struct Triangle {
// 	Point a, b, c;
// 	int index[3];
// 	Triangle(Point _x, Point _y, Point _z) {
// 		a = _x;
// 		b = _y;
// 		c = _z;
// 	}
// 	Triangle(Point _x, Point _y, Point _z, int i, int j, int k):a(_x), b(_y), c(_z) {
// 		index[0] = i;
// 		index[1] = j;
// 		index[2] = k;
// 	}
// 	Triangle() {
		
// 	}
// };

// struct Rect {
// 	int minX, minY;
// 	int maxX, maxY;
// 	Rect(minX, maxX, minY, maxY):minX(minX), maxX(maxX), minY(minY), maxY(maxY){}
// };

// void face_detect(string path, vector<Point>& feature);
void readPoint(string path, std::vector<Point>& points);
void addExtraPoint(CImg<float>& img, vector<Point>& points);
bool isPointInCircle(Point& point, Triangle& tri);
void getTriangleList(vector<Point>& points, vector<Triangle>& TriangleList);
void drawTriangle(CImg<float> img, vector<Triangle>& TriangleList);
Triangle* getTempTriangle(Triangle& tri1, Triangle& tri2, float alpha);
void getAffineMatrix(Triangle& src, Triangle& dst, MatrixXf& mat);
void morph(CImg<float>& srcimg, CImg<float>& dstimg, CImg<float>& result, 
	Triangle& src, Triangle& temp, Triangle& dst, float alpha);
void Morphing(CImg<float>& src, CImg<float>& dst, int mode);
Triangle* findTri(Triangle& tri, vector<Triangle>& TriangleList);


// triangle function
float cross3(const Point &a, const Point &b, const Point &p);
bool isPointInTriangle(const Point& p, const Triangle& tri);

#endif