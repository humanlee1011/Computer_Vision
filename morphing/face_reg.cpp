/*
	使用开源库dlib进行人脸特征点识别

	author：Leexy
	Time：2018-05-14
 */




// #include <dlib/image_processing/frontal_face_detector.h>
// #include <dlib/image_processing/render_face_detections.h>
// #include <dlib/image_processing.h>
// #include <dlib/gui_widgets.h>
// #include <dlib/image_io.h>
#include <Eigen/Dense>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <CGAL/Delaunay_triangulation_2.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include "CImg.h"


#include "face_reg.h"

#define N_FEATURES 68

// using namespace dlib;
using namespace std;
using namespace cimg_library;
using namespace Eigen;

#define Point Vector2<float>
#define Triangle Triangle<float>


// void face_detect(string path, vector<Point>& feature) {
// 	// We need p1 face detector, we will use this to get bounding boxes for
// 	// each face in an image.
// 	frontal_face_detector detector = get_frontal_face_detector();
// 	// And we also need p1 shape_predictor.  This is the tool that will predict face
//     // landmark positions given an image and face bounding box.  Here we are just
//     // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
//     // as p1 command line argument.
//     array2d<rgb_pixel> img;
//     load_image(img, path.c_str());
//     shape_predictor sp;
//     deserialize("shape_predictor_68_face_landmarks.dat") >>sp; 		
//     //detect face
//     vector<rectangle> dets = detector(img);
//     //detect face feature	
//     full_object_detection shape = sp(img, dets[0]);
//     // draw img1 face feature point
//   	float red[3] = {255.0f, 0.0f, 0.0f};
//   	if (!shape.empty()) {
//   		for (int i = 0; i < N_FEATURES; i++) {
//   			int x = shape[0].parts(i).x();
//   			int y = shape[0].parts(i).y();
//   			img.draw_circle(x, y, 3, red);
//   			feature.push_back(Point(x, y))
//   		}
//   	}
// }

void readPoint(string path, vector<Point>& points) {
	// format: x, y\n
	ifstream in(path.c_str());
	if (!in.is_open()) {
		cout << "Error opening filen";
		exit(1);
	}
	cout << path << endl;
	while (!in.eof()) {
		int x, y;
		in >> x >> y;
		points.push_back(Point(x, y));
		//cout << x << " " << y << endl;
	}
	cout << "size: " << points.size() << endl;
}

void addExtraPoint(CImg<float>& img, vector<Point>& points) {
	points.push_back(Point(0, 0));
	points.push_back(Point(0, img._height / 2));
	points.push_back(Point(0, img._height));
	points.push_back(Point(img._width / 2, 0));
	points.push_back(Point(img._width / 2, img._height));
	points.push_back(Point(img._width, 0));
	points.push_back(Point(img._width, img._height / 2));
	points.push_back(Point(img._width, img._height));
}



// bool isPointInCircle(Point& point, Triangle& tri) {
// 	double radius_2;
// 	double cntrx, cntry;

// 	double a1 = (tri.p2.x - tri.p1.x);
// 	double b1 = (tri.p2.y - tri.p1.y);
// 	double c1 = (a1 * a1 + b1 * b1) / 2.0;

// 	double a2 = (tri.p3.x - tri.p1.x);
// 	double b2 = (tri.p3.y - tri.p1.y);
// 	double c2 = (a2 * a2 + b2 * b2) / 2.0;

// 	double d = (a1 * b2 - a2 * b1);
// 	cntrx = tri.p1.x + (c1 * b2 - c2 * b1) / d;
// 	cntry = tri.p1.x + (a1 * c2 - a2 * c1) / d;
// 	radius_2 = pow(tri.p1.x - cntrx, 2.0) + pow(tri.p1.y - cntry, 2.0);
// 	double point_r = pow(point.x - cntrx, 2.0) + pow(point.y - cntry, 2.0);
// 	if (point_r <= radius_2)
// 		return true;
// 	else 
// 		return false;
// }

// 借鉴前辈的方法
// void getTriangleList(vector<Point>& points, vector<Triangle>& TriangleList) {
// 	if (points.size() < 3) 
// 		return ;
// 	int size = points.size();
// 	for (int i = 0; i < size - 2; i++) {
// 		for (int j = i + 1; j < size - 1; j++) {
// 			for (int k = j + 1; k < size; k++) {
// 				Point p1 = points[i];
// 				Point p2 = points[j];
// 				Point p3 = points[k];

// 				Triangle tri(p1, p2, p3, i, j, k);
// 				bool no_exist = true;
// 				for (int m = 0; m < size; m++) {
// 					Point p = points[m];
// 					if (m == i || m == j || m == k) continue;
// 					double k1 = (double)(p1.y - p2.y) / (p1.x - p2.x);
// 					double k2 = (double)(p1.y - p3.y) / (p1.x - p3.x);
// 					if (isPointInCircle(p, tri) || abs(k1 - k2) < 0.1) {
// 						no_exist = false;
// 						break;
// 					}

// 				}
// 				if (no_exist)
// 					TriangleList.push_back(tri);
// 			}
// 		}
// 	}
// }

void _drawTriangle(CImg<float>& img, Triangle& Tri) {
	float red[3] = {255, 0, 0};
	img.draw_line((int)Tri.p1.x, (int)Tri.p1.y,
	 (int)Tri.p2.x, (int)Tri.p2.y, red);
	img.draw_line((int)Tri.p1.x, (int)Tri.p1.y,
	 (int)Tri.p3.x, (int)Tri.p3.y, red);
	img.draw_line((int)Tri.p3.x, (int)Tri.p3.y,
	 (int)Tri.p2.x, (int)Tri.p2.y, red);
	
}
// draw triangle
void drawTriangle(CImg<float> img, vector<Triangle>& TriangleList) {
	float red[3] = {255, 0, 0};
	for (int i = 0; i < TriangleList.size(); i++) {
		//cout << TriangleList[i] << endl;
		_drawTriangle(img, TriangleList[i]);
	}
	img.display("Triangle");
}



void drawPoint(CImg<float> img, vector<Point>& points) {
	float red[3] = {255, 0, 0};
	for (int i = 0; i < points.size(); i++) {
		img.draw_circle((int)points[i].x, (int)points[i].y, 3, red);
	}
	img.display("Feature points");
}

Triangle* getTempTriangle(Triangle& tri1, Triangle& tri2, float alpha) {
	// after mapping, x, y should be integer
	int x1 = (1 - alpha) * tri1.p1.x + alpha * tri2.p1.x;
	int y1 = (1 - alpha) * tri1.p1.y + alpha * tri2.p1.y;

	int x2 = (1 - alpha) * tri1.p2.x + alpha * tri2.p2.x;
	int y2 = (1 - alpha) * tri1.p2.y + alpha * tri2.p2.y;

	int x3 = (1 - alpha) * tri1.p3.x + alpha * tri2.p3.x;
	int y3 = (1 - alpha) * tri1.p3.y + alpha * tri2.p3.y;
	return new Triangle(Point(x1, y1), Point(x2, y2), Point(x3, y3));
}

void getAffineMatrix(Triangle& src, Triangle& dst, MatrixXf& mat) {
	MatrixXf UV(6, 1);
	UV << dst.p1.x, dst.p1.y, dst.p2.x, dst.p2.y, dst.p3.x, dst.p3.y;
	MatrixXf A(6, 6);
	A << src.p1.x, src.p1.y, 1, 0, 0, 0,
		0, 0, 0, src.p1.x, src.p1.y, 1, 
		src.p2.x, src.p2.y, 1, 0, 0, 0,
		0, 0, 0, src.p2.x, src.p2.y, 1,
		src.p3.x, src.p3.y, 1, 0, 0, 0,
		0, 0, 0, src.p3.x, src.p3.y, 1;
	A = A.inverse();
	MatrixXf M(6, 1);
	M = A * UV;
	mat << M(0, 0), M(1, 0), M(2, 0), M(3, 0), M(4, 0), M(5, 0), 0, 0, 1;
}

// refer to vetor cross to 
float cross3(const Point &a, const Point &b, const Point &p) {
    return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
}
bool isPointInTriangle(const Point& p, const Triangle& tri) {
    if (cross3(tri.p1, tri.p2, p) >= 0 && cross3(tri.p2, tri.p3, p) >= 0
     && cross3(tri.p3, tri.p1, p) >= 0)
        return true;
    else if (cross3(tri.p1, tri.p2, p) <= 0 && cross3(tri.p2, tri.p3, p) <= 0
     && cross3(tri.p3, tri.p1, p) <= 0)
        return true;
    else
        return false;
}

Triangle* findTri(Triangle& tri, vector<Triangle>& TriangleList) {
	for (int i = 0; i < TriangleList.size(); i++) {
		//cout << i << endl;
		if (tri.equal(TriangleList[i])) {
			return new Triangle(TriangleList[i]);
		}
	}
	return NULL;
}

// transformation between multiple pair of lines 
// For each pixel X in the destination 
//         DSUM=0 
//         weightsum = 0 
//         For each line PiQi 
//              calculate u, v based on PiQi 
//              calculate X’i based on u, v and P’iQ’i 
//              calculate displacement Di=X’i-Xi for this line 
//              dist= shortest distance from X to PiQi 
//              weight = (lengthP / (p1 + dist))p2 
//              DSUM  += Di * weight 
//              weightsum += weigth 
//         X’ = X + DSUM / weightsum 
//         destinationImage(X) = sourceImage(X’)

void morph(CImg<float>& srcimg, CImg<float>& dstimg, CImg<float>& result, Triangle& src, Triangle& temp, Triangle& dst, float alpha) {

	// 对于一个三角形，先找到他的外接矩形，然后遍历矩阵内的点，看矩阵的点是否在三角形内
	// 对三角形内的点进行仿射变换
	int minX = min(temp.p1.x, min(temp.p2.x, temp.p3.x));
	int minY = min(temp.p1.y, min(temp.p2.y, temp.p3.y));
	int maxX = max(temp.p1.x, max(temp.p2.x, temp.p3.x));
	int maxY = max(temp.p1.y, max(temp.p2.y, temp.p3.y));

	//cout << "1: " <<  src << endl <<" 2: " << dst << endl;

	MatrixXf src2temp(3, 3), dst2temp(3, 3);
	getAffineMatrix(src, temp, src2temp);
	getAffineMatrix(dst, temp, dst2temp);
	src2temp = src2temp.inverse();
	dst2temp = dst2temp.inverse();

	// cout << src2temp << endl;
	// cout << dst2temp << endl;
	float red[3] = {255, 0, 0};
	CImg<float> ttt(srcimg);
	_drawTriangle(ttt, src);
	for (int u = minX; u <= maxX; u++) {
		for (int v = minY; v <= maxY; v++) {
			Point point(u, v);
			if (isPointInTriangle(point, temp)) {
				ttt(u, v) = 0;
				MatrixXf uv(3, 1);
				uv << u, v, 1;
				MatrixXf xy = src2temp * uv;

				float sx = xy(0, 0) / xy(2, 0);
				float sy = xy(1, 0) / xy(2, 0);

				xy = dst2temp * uv;
				float dx = xy(0, 0) / xy(2, 0);
				float dy = xy(1, 0) / xy(2, 0);
				// 判断出界
				if (sx >= srcimg._width - 1 || sx <= 0 || sy >= srcimg._height - 1 || sy <= 0)
	      			continue;
	      		if (dx >= dstimg._width - 1 || dx <= 0 || dy >= dstimg._height - 1 || dy <= 0)
	      			continue;
	      		result(u, v, 0) = srcimg(sx, sy, 0) * (1 - alpha) + dstimg(dx, dy, 0) * alpha;
	      		result(u, v, 1) = srcimg(sx, sy, 1) * (1 - alpha) + dstimg(dx, dy, 1) * alpha;    
	      		result(u, v, 2) = srcimg(sx, sy, 2) * (1 - alpha) + dstimg(dx, dy, 2) * alpha;
	      	}
		}
	}
	//ttt.display();
}

void Morphing(CImg<float>& src, CImg<float>& dst, int mode) {
	//step-1：找到人脸特征点

	string fea = "11.txt";
	string fea2 = "22.txt";
	vector<Point> feature1, feature2;
	readPoint(fea, feature1);
	readPoint(fea2, feature2);
	addExtraPoint(src, feature1);
	addExtraPoint(dst, feature2);
	drawPoint(src, feature1);
	drawPoint(dst, feature2);	
	if (feature1.size() != feature2.size()) {
		cout << "the size differs!\n" << feature1.size() << " " << fea2.size();
		return;
	}
	//step-2：根据特征点进行三角形剖分（Delaunay三角剖分算法）
	Delaunay<float> srcdt, dstdt;

	vector<Triangle> srcTri = srcdt.triangulate(feature1);
	vector<Triangle> dstTri = dstdt.triangulate(feature2);

	drawTriangle(src, srcTri);
	drawTriangle(dst, dstTri);

	// cout << "??????.n";
	//step-3：对每个三角形进行morph变形
	for (int i = 0; i <= 10; i++) {
		float alpha = i * 0.1;
		int width = (1 - alpha) * src._width + alpha * dst._width;
		int height = (1 - alpha) * src._height + alpha * dst._height;
		CImg<float> result(width, height, 1, 3);
		//对每一组三角形进行morph变换
		cout << "srcTri.size() " << srcTri.size() << endl;

		cout << "dstTri.size() " << dstTri.size() << endl;
		int cnt = 0;
		for (int i = 0; i < srcTri.size(); i++) {
			Triangle* dstT = findTri(srcTri[i], dstTri);
			if (dstT) {
				Triangle* temp = getTempTriangle(srcTri[i], *dstT, alpha);
				// cout << "srcTri:" << srcTri[i] << endl;
				// cout << "temp:" << temp << endl;
				// cout << "dst: " << (*dstT) << endl;
				//cout << temp << endl;
				morph(src, dst, result, srcTri[i], *temp, *dstT, alpha);
				cnt++;
			}
		}
		cout << "alpha: " << alpha << " cnt: " << cnt << endl;
		result.display();
		result.save(("result/" + to_string(i)).c_str());
	}
	cout << "???.n";
}