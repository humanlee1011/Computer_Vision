#include <iostream>
#include <vector>
#include <string>
#define cimg_imagemagick_path "C:/Program Files/ImageMagick-7.0.7-Q16"
#include "CImg.h"
#include "face_reg.h"


using namespace std;
using namespace cimg_library;


int main() {
	string root = "./dataset/";
	int mode = 0;
	cin >> mode;
	if (mode) {
		CImg<float> img1((root + "girl.bmp").c_str());
		CImg<float> img2((root + "boy.bmp").c_str());
		img1.display();
		img2.display();
		Morphing(img1, img2, mode);
	}
	else {
		CImg<float> img1((root + "1.bmp").c_str());
		CImg<float> img2((root + "2.bmp").c_str());
		img1.display();
		img2.display();
		Morphing(img1, img2, mode);
	}
	
}