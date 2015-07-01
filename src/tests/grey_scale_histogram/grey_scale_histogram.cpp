// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <string>
#include "./grey_scale_histogram.h"
#include "../../libraries/CImg.h"

static void
write_values_2_file(unsigned int *values, std::string filename);

/*
 * Create data files for a grey scale histogram
 * and returns the average grey scale value of the given image.
 */
double
run_test_grsh(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  unsigned int gs_values[kTonalRange] = {0};
  unsigned int number_of_pixels = image.width() * image.height();

  // scan each pixel and count there grey scale values
  double sum = 0;
  cimg_forXY(image, x, y) {
    gs_values[image(x, y, 0, 0)]++;
    sum += static_cast<double>(image(x, y, 0, 0));
  }

  std::string path = img_info->outdir + "" + TEST_GRSH + "/";
  mkdir(path.c_str(), kDefaultRights);

  std::string path_filename_prefix
    = path + get_filename(img_info->encrypted) + "_";
  write_values_2_file(gs_values, path_filename_prefix + kGrshDataFileName);
  return sum / static_cast<double>(number_of_pixels);
}

/*
 * Writes values to data file.
 */
static void
write_values_2_file(unsigned int *values, std::string filename) {
  std::ofstream data_file(filename);
  if (data_file.is_open()) {
    for (int i = 0; i < kTonalRange; i++) {
      data_file << i << " " << values[i] << "\n";
    }
  } else {
    printf("data could not be opened.");
  }
  data_file.close();
}
