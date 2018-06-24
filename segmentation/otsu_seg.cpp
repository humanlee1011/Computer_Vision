/*
	otsu_seg.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include "CImg.h"
#include <cmath>

using namespace std;
using namespace cimg_library;

class OTSUSegementation {
public:
	OTSUSegementation(CImg<float> & img) {
		_img = img;
	}

	float getThreshold() {
		return _threshold;
	}

	void calculateHist() {
		for (int i = 0; i < 256; i++)
			histogram[i] = 0;
		cimg_forXY(_img, x, y) {
			histogram[(int)_img(x, y)]++;	
		}
	}

	void segmentation() {
		float totalPixel = _img._height * _img._width;
		int gray = 0, grayB = 0, grayF = 0;
		float wB = 0, wF = 0;
		float max_var = 0;
		float temp_var = 0;
		int numB = 0, numF = 0;

		calculateHist();
		for (int i = 0; i < 256; i++) {
			gray += i * histogram[i];
		}

		for (int i = 0; i < 256; i++) {
			grayB += histogram[i] * i;// 总灰度
			grayF = gray - grayB;// 剩余灰度
			numB += histogram[i];// 背景占图像的点数	
			numF = totalPixel - numB;
			wB = numB / totalPixel;
			wF = 1 - wB;		
			//cout << "numB: " << numB << " wF: " << wF << endl;
			if (wB == 0 || wF == 0)
				continue;
			temp_var = wB * wF * (grayB / numB - grayF / numF) * (grayB / numB - grayF / numF);
			if (temp_var > max_var) {
				_threshold = i;
				max_var = temp_var;
			}
		}
		cout << "T: " << _threshold << endl; 
		// return _threshold;
	}

	void display(string name) {
		CImg<float> result(_img._width, _img._height, 1);
		cimg_forXY(_img, x, y) {
			if (_img(x, y) > _threshold) {
				result(x, y, 0, 0) = 0;
			} else {
				result(x, y, 0, 0) = 255;
			}
		}
		//result.save_jpeg(("result/otsu/" + name + ".jpg").c_str());
		//result.display(name.c_str());
	}
private:
	CImg<float> _img;
	float _threshold;
	int histogram[256];
};