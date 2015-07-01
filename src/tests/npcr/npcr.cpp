// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./npcr.h"
#include "../../libraries/CImg.h"

double
run_test_npcr(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image1(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> image2(img_info->encrypted.c_str());

  double number_of_pixels = image1.width() * image1.height();
  double npcr = 0;

  // just compare images with the same size, stop if not
  if (image1.width() != image2.width()
      || image1.height() != image2.height())
    return -1.0;

  // calculate npcr = (1/number_of_pixel) * sum(D(i,j) * 100
  // D(i,j) = 1 , if image1(i,j) != image2(i,j)
  // D(i,j) = 0 , if image1(i,j) == image2(i,j)
  cimg_forXY(image1, x, y) {
    if (image1(x, y, 0, 0) != image2(x, y, 0, 0))
      npcr++;
  }
  return (npcr * 100) / number_of_pixels;
}
