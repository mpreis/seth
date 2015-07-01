// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./grey_scale_value.h"
#include "../../libraries/CImg.h"

double
run_test_grsv(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  unsigned int gs_values[kTonalRange] = {0};
  double number_of_pixels = image.width() * image.height();
  double average  = 0.0;
  double variance = 0.0;

  // scan each pixel and count there grey scale values
  cimg_forXY(image, x, y) {
    gs_values[image(x, y, 0, 0)]++;
  }

  // calculate average
  for (int i = 0; i < kTonalRange; i++)
    average += gs_values[i];
  average /= kTonalRange;

  // calculate variance
  for (int i = 0; i < kTonalRange; i++)
    variance = pow((gs_values[i] - average), 2);
  variance /= kTonalRange - 1;

  // calculate entropy
  for (int i = 0; i < kTonalRange; i++)
      average += gs_values[i] * i;
  average /= number_of_pixels;

  return variance;
}
