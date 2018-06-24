#include "extract_features.h"
#include "warping.h"
#include "blending.h"
#include "stitching.h"


// return image's gray image
CImg<float> get_gray_image(CImg<float>& img) {
  CImg<float> res(img._width, img._height);
  cimg_forXY(img, x, y) {
    res(x, y) = 0.299 * img(x, y, 0, 0) +
                0.587 * img(x, y, 0, 1) +
                0.114 * img(x, y, 0, 2);
  }
  return res;
}

void ReplacePairs(vector<POINT_PAIR>& bigger, vector<POINT_PAIR>& smaller) {
  smaller.clear();
  for (int i = 0; i < bigger.size(); i++) {
    POINT_PAIR temp(bigger[i].b, bigger[i].a);
    smaller.push_back(temp);
  }
}


vector<int> getAvgOffset(const vector<POINT_PAIR>& pairs, vector<int>& idxs) {
  int offset_x = 0;
  int offset_y = 0;
  int min_x = 10000;
  int min_y = 10000;

  int size = idxs.size();
  int cnt = 0;
  for (int i = 0; i < size; i++) {

    int diff_x = pairs[i].a.x - pairs[i].b.x;
    int diff_y = pairs[i].a.y - pairs[i].b.y;
    if (diff_x == 0 || diff_y == 0 || abs(diff_y) > 200)
      continue;
    offset_x += diff_x;
    offset_y += diff_y;
    cnt++;
    if (pairs[i].a.x < min_x)
      min_x = pairs[i].a.x;
    if (pairs[i].a.y < min_y)
      min_y = pairs[i].a.y;
  }
  offset_x /= cnt;
  offset_y /= cnt;
  int ans_x = 0;
  int ans_y = 0;
  cnt = 0;
  for (int i = 0; i < size; i++) {
    int diff_x = pairs[i].a.x - pairs[i].b.x;
    int diff_y = pairs[i].a.y - pairs[i].b.y;
    cout << "diff_x " << diff_x << " diff_y " << diff_y << endl;
    // if (diff_x == 0 || diff_y == 0)
    //   continue;
    if (abs(diff_x - offset_x) < abs(offset_x) / 2 && (abs(diff_y - offset_y) < abs(offset_y) / 2 || abs(offset_y) / 2 < 4)) {
      ans_x += diff_x;
      ans_y += diff_y;
      cnt++;
    }
  }
  cout << "Threshold: offset_x " << offset_x << " offset y " << offset_y << endl;
  if (cnt) {
    ans_x /= cnt;
    ans_y /= cnt;
  }
  vector<int> res;
  res.push_back(ans_x);
  res.push_back(ans_y);
  res.push_back(min_x);
  res.push_back(min_y);
  return res;
}


CImg<float> stitching(vector<CImg<float> >& src_imgs) {
  // Used to save each image's features and corresponding coordinates.
  vector<map<vector<float>, VlSiftKeypoint> > features(src_imgs.size());

  // Preprocessing input images
  for (int i = 0; i < src_imgs.size(); i++) {
    // Cylinder projection
    // src_imgs[i] = resize(src_imgs[i]);
    cout << "Cylinder Projection start.\n";
    src_imgs[i] = CylinderProjection(src_imgs[i]);
    cout << "Cylinder Projection ended.\n";

    // using gray images to promote efficiency
    CImg<float> gray = get_gray_image(src_imgs[i]);

    cout << "extract_features start.\n";
    features[i] = extractFeatures(gray);
    cout << "extract_features ended.\n";
  }

  // Used to record the image's adjacent images.
  bool adjacent[20][20] = { false };

  // Used to record each image's adjacent images.
  vector<vector<int> > matching_index(src_imgs.size());


  // Find adjacent images
  cout << "Find adjacent images.\n";
  for (int i = 0; i < src_imgs.size(); i++) {
    for (int j = i + 1; j < src_imgs.size(); j++) {
      // TODO: j = i + 1
      // if (i == j) cpntinue;

      vector<POINT_PAIR> pairs = getPointPairsFromFeatures(features[i], features[j]);
      // TODO: why the threshold equals 20
      if (pairs.size() >= 20) {
        adjacent[i][j] = true;
        // TODO: record the pairs and don't compute again
        matching_index[i].push_back(j);
        cout << "Image " << i << " and " << j << " are adjacent." << endl;
      }
    }
  }
  cout << endl;

  cout << "Stitching start.\n";

  // TODO: getMiddleIndex
  int start_idx = 0;

  int prev_idx = start_idx;
  queue<int> unstitched_idx;

  unstitched_idx.push(start_idx);

  CImg<float> cur_stitched_img = src_imgs[start_idx];

  while (!unstitched_idx.empty()) {
    int src_idx = unstitched_idx.front();
    unstitched_idx.pop();

    for (int i = 0; i < matching_index[src_idx].size(); i--) {
      int dst_idx = matching_index[src_idx][i];

      if (adjacent[src_idx][dst_idx]) {
        adjacent[src_idx][dst_idx] = adjacent[dst_idx][src_idx] = false;
        unstitched_idx.push(dst_idx);

        cout << "get Point Pairs from features.\n";
        // duplicate find pairs
        // Matching features using best-bin kd-tree
        vector<POINT_PAIR> src_to_dst_pairs = getPointPairsFromFeatures(features[src_idx], features[dst_idx]);
        vector<POINT_PAIR> dst_to_src_pairs = getPointPairsFromFeatures(features[dst_idx], features[src_idx]);

        // Find the best matching direction
        if (src_to_dst_pairs.size() > dst_to_src_pairs.size())
          ReplacePairs(src_to_dst_pairs, dst_to_src_pairs);
        else
          ReplacePairs(dst_to_src_pairs, src_to_dst_pairs);

        // Finding Homography by RANSAC
        cout << "RANSAC.\n";
        // Get the moving offset_x and offset_y

        vector<int> idxs = RANSAC(dst_to_src_pairs);
        // MatrixXf backward_H = RANSAC(src_to_dst_pairs);
        // check(cur_stitched_img, src_imgs[dst_idx], pairs, idxs);
        // CImg<float> temp1(cur_stitched_img);
        // CImg<float> temp2(src_imgs[dst_idx]);
        // float red[3] = {255, 0, 0};
        // for (int i = 0; i < idxs.size(); i++) {
        //   temp1.draw_circle(src_to_dst_pairs[i].a.x, src_to_dst_pairs[i].a.y, 3, red);
        //   temp2.draw_circle(src_to_dst_pairs[i].b.x, src_to_dst_pairs[i].b.y, 3, red);
        // }
        // temp1.display("temp1");
        // temp2.display("temp2");

        // Get average offset x and offset y
        vector<int> params = getAvgOffset(src_to_dst_pairs, idxs);
        // int offset_y = getAvgOffsetY(src_to_dst_pairs, idxs);
        cout << "Before: offset_x " << params[0] << " offset_y" << params[1] << endl;
        // warpingFeaturesPointByOffset(features[dst_idx], params[0], params[1]);

        cur_stitched_img = blendTwoImages(cur_stitched_img, src_imgs[dst_idx], params[0], params[1], params[2], params[3]);
        cur_stitched_img.display("middle");
        prev_idx = dst_idx;
      }
    }
  }
  return cur_stitched_img;
}