/*
	iteraton_seg.cpp
 */ 
#include <iostream>
#include "CImg.h"
#include <vector>
#include <string>
#include <cmath>

using namespace std;
using namespace cimg_library;

class IterationSegmentation {
public:
	IterationSegmentation(CImg<float>& img, float _d) {
		_img = img;//首先假设这是一个灰度图
		_T = 0;
		deltaT = _d;
	}

	void calculateHist() {
		// init
		for (int i = 0; i < 256; i++) 
			histogram[i] = 0;
		cimg_forXY(_img, x, y) {
			histogram[(int)_img(x, y)]++;
		}
	}
	void segmentation() {
		// initial estimate of T
		calculateHist();
		float Tn = 100;
		//cout << "T" << _T << "Tn" << Tn << "delta" << deltaT << endl;
		while (abs(_T - Tn) > deltaT) {
			_T = Tn;
			float m1 = 0, m2 = 0;
			int cnt1 = 0, cnt2 = 0;
			//cout << "Enter";
			for (int i = 0; i < 256; i++) {
				if (i < _T) {
					m1 += histogram[i] * i;
					cnt1 += histogram[i];
				}
				else {
					m2 += histogram[i] * i;
					cnt2 += histogram[i];
				}
			}
			//cout << "cnt1: " << cnt1 << "m2: " << cnt2 << endl;
			m1 /= cnt1;
			m2 /= cnt2;
			//cout << "m1: " << m1 << "m2: " << m2 << endl;
			Tn = (m1 + m2) / 2;
		}
		cout << "T: " << _T << endl;
		//return _T;
	}

	float getThreshold() {
		return _T;
	}

	void display(string name) {
		CImg<float> result(_img._width, _img._height, 1);
		cimg_forXY(_img, x, y) {
			if (_img(x, y) > _T) {
				result(x, y, 0, 0) = 0;
			} else {
				result(x, y, 0, 0) = 255;
			}
		}
		//result.save_jpeg(("result/iter/" + name + ".jpg").c_str());
		//result.display(name.c_str());
	}

private:
	float _T;
	float deltaT;
	int histogram[256];
	CImg<float> _img;
};
