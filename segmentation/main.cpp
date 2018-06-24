/*
	main.cpp
 */

#include <iostream>
#include <string>
#include <time.h>
#define cimg_convert_path "C:/Program Files (x86)/ImageMagick-7.0.7-Q16/convert.exe"
#include "CImg.h"
#include "iteration_seg.cpp"
#include "otsu_seg.cpp"


using namespace std;
using namespace cimg_library;


int main() {
	string root = "dataset/img";
	float d;
	cin >> d;
	float avg_iter = 0, avg_ostu = 0;
	// cimg::imagemagick_path("C:\\Program Files (X86)\\ImageMagick-7.0.7-Q16\\convert.exe");
	for (int i = 1; i <= 100; i++) {

		CImg<float> img((root + to_string(i) + ".jpg").c_str());
		CImg<float> gray(img._width, img._height, 1, 1);
		// Gray = R*0.299 + G*0.587 + B*0.114
		cimg_forXY(img, x, y) {
			gray(x, y, 0, 0) = 0.299 * img(x, y, 0, 0) + 0.587 * img(x, y, 0, 1) + 0.114 * img(x, y, 0, 2);
		}
		// CImg<int> GrayImg = CImg<int>(img._width, img._height, 1, 1, 0);  
		// 34、38、41、55、57、58、59、
		// if (i == 2 || i == 3 || i == 18 || i == 24 || i == 32 || i == 39 || i == 92) {
		// 	CImg<int> histImg = gray.get_histogram(256, 0, 255);
		// 	histImg.display_graph("Histogram", 3);
		// }
		
		//histImg.save(("result/histogram/hist" + to_string(i) + ".jpg").c_str());
		//gray.display();
		//Iteration segmentation test
		//img.display();
		time_t s = clock();
		IterationSegmentation Iter(gray, d);
		Iter.segmentation();
		time_t e = clock();
		Iter.display("Iter" + to_string(i));
		avg_iter += e - s;
		cout << "Time: " << e - s << "ms" << endl;
		// otsu segmentation test
		s = clock();
		OTSUSegementation OTSU(gray);
		OTSU.segmentation();
		e = clock();
		avg_ostu += e - s;
		cout << "Time: " << e - s << "ms" << endl;
		OTSU.display("Otsu" + to_string(i));

	}
	cout << "Result: \n" << "Iteration Average Time: " << avg_iter / 100
		<< endl << "Ostu Average Time: " << avg_ostu / 100 << endl; 
} 