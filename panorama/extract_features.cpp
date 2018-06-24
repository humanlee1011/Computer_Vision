#include "extract_features.h"



map<vector<float>, VlSiftKeypoint> extractFeatures(CImg<float>& img) {
  CImg<float> src(img);
  float resize_factor;
  int width = src._width;
  int height = src._height;
  // resize the image to reduce the computation memory
  if (width < height) {
    resize_factor = RESIZE_SIZE / width;
  } else {
    resize_factor = RESIZE_SIZE / height;
  }

  if (resize_factor >= 1) {
    resize_factor = 1;
  } else {
    src.resize(width * resize_factor, height * resize_factor, src.spectrum(), 3);
  }

  vl_sift_pix *imageData = new vl_sift_pix[src._width * src._height];

  for (int i = 0; i < src.width(); i++) {
    for (int j = 0; j < src.height(); j++) {
      imageData[j * src.width() + i] = src(i, j, 0);
    }
  }

  // Setting SIFT filter params
  int noctaves = 4;
  int nlevels = 2;
  int o_min = 0;
  // Initialize a SIFT filter object
  VlSiftFilt *sf = NULL;
  sf = vl_sift_new(src.width(), src.height(), noctaves, nlevels, o_min);

  map<vector<float>, VlSiftKeypoint> features;

  // Compute the first octave of the DOG scale space
  if (vl_sift_process_first_octave(sf, imageData) != VL_ERR_EOF) {
    while (true) {
      // Run the SIFT detector to get the keypoints.
      vl_sift_detect(sf);
      VlSiftKeypoint *pKeyPoint = sf->keys;
      //For each keypoint
      for (int i = 0; i < sf->nkeys; i++) {
        VlSiftKeypoint tempKp = *pKeyPoint;

        // Get the keypoint orientation(s).
        double angles[4];
        int angleCount = vl_sift_calc_keypoint_orientations(sf, angles, &tempKp);

        // For each orientation
        for (int j = 0; j < angleCount; j++) {
          double tempAngle = angles[j];
          vl_sift_pix descriptors[128];

          // Get the keypoint descriptor
          vl_sift_calc_keypoint_descriptor(sf, descriptors, &tempKp, tempAngle);

          vector<float> des;
          int k = 0;
          while (k < 128) {
            des.push_back(descriptors[k]);
            k++;
          }

          tempKp.x /= resize_factor;
          tempKp.y /= resize_factor;
          tempKp.ix = tempKp.x;
          tempKp.iy = tempKp.y;

          features.insert(make_pair(des, tempKp));
        }

        pKeyPoint++;
      }
      if (vl_sift_process_next_octave(sf) == VL_ERR_EOF) {
        break;
      }
    }
  }

  vl_sift_delete(sf);

  delete[] imageData;
  imageData = NULL;

  return features;
}


