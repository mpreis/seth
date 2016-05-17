// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./uaci.h"
#include "../../libraries/CImg.h"

double
run_test_uaci(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image1(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> image2(img_info->encrypted.c_str());

  double number_of_pixels = image1.width() * image1.height();
  double uaci = 0;

  // just compare images with the same size, stop if not
  if (image1.width() != image2.width() || image1.height() != image2.height())
    return -1.0;

  // calculate uaci
  // = (1/number_of_pixel) * sum(abs(image1(i,j) - image2(i,j))/T) * 100
  cimg_forXY(image1, x, y) {
    double i1 = static_cast<double>(image1(x, y, 0, 0));
    double i2 = static_cast<double>(image2(x, y, 0, 0));
    double value = i1 - i2;
    uaci += (std::abs(value)/kTonalRange);
  }
  return (uaci * 100) / number_of_pixels;
}
