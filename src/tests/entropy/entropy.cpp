// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./entropy.h"
#include "../../libraries/CImg.h"

double
run_test_entr(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  unsigned int gs_values[kTonalRange] = {0};
  double number_of_pixels = image.width() * image.height();
  double entropy = 0.0;

  // scan each pixel and count there grey scale values
  cimg_forXY(image, x, y) {
    gs_values[image(x, y, 0, 0)]++;
  }

  // calculate entropy
  for (int i = 0; i < kTonalRange; i++)
    if (gs_values[i] > 0)
      entropy += (gs_values[i]/number_of_pixels)
                  * (log2(number_of_pixels/gs_values[i]));

  return entropy;
}

double
run_test_entr_img(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  unsigned int gs_values[kTonalRange] = {0};
  double entropy_img = 0.0;

  // scan each pixel and count there grey scale values
  cimg_forXY(image, x, y) {
    gs_values[image(x, y, 0, 0)]++;
    // calc image entropy
    if (image(x, y, 0, 0) != 0)
      entropy_img += (static_cast<double>(image(x, y, 0, 0))/kTonalRange)
                  *(log2(static_cast<double>(kTonalRange)/image(x, y, 0, 0)));
  }
  return entropy_img;
}
