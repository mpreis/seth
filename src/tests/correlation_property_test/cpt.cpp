// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <string>
#include "./cpt.h"
#include "../../libraries/CImg.h"

static void
set_random_values(cimg_library::CImg<unsigned int> image,
  struct couple_t *values, int dimension);

static double
calculate_e(struct couple_t *values, get_value_func_t get_value);

static double
calculate_d(double e, struct couple_t *values, get_value_func_t get_value);

static double calculate_cov(double e_x, double e_y, struct couple_t *values);
static double calculate_r(struct couple_t *values);
static int get_pixel(struct couple_t *values, int i);
static int get_adjacent_pixel(struct couple_t *values, int i);
static void write_values_2_file(struct couple_t *values, std::string filename);

double
run_test_cprt_vertical(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  struct couple_t v_couples[kNrOfCouples];
  set_random_values(image, v_couples, kDimVertical);

  std::string path = img_info->outdir + "cprt/";
  mkdir(path.c_str(), kDefaultRights);

  std::string path_filename_prefix
    = path + get_filename(img_info->encrypted) + "_";
  write_values_2_file(v_couples, path_filename_prefix + kCprtDataFileNameV);

  return calculate_r(v_couples);
}

double
run_test_cprt_horizontal(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  struct couple_t h_couples[kNrOfCouples];
  set_random_values(image, h_couples, kDimHorizontal);

  std::string path = img_info->outdir + "cprt/";
  mkdir(path.c_str(), kDefaultRights);

  std::string path_filename_prefix
    = path + get_filename(img_info->encrypted) + "_";
  write_values_2_file(h_couples, path_filename_prefix + kCprtDataFileNameH);

  return calculate_r(h_couples);
}

double
run_test_cprt_diagonal(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  struct couple_t d_couples[kNrOfCouples];
  set_random_values(image, d_couples, kDimDiagonal);

  std::string path = img_info->outdir + "cprt/";
  mkdir(path.c_str(), kDefaultRights);

  std::string path_filename_prefix
    = path + get_filename(img_info->encrypted) + "_";
  write_values_2_file(d_couples, path_filename_prefix + kCprtDataFileNameD);

  return calculate_r(d_couples);
}

/*
 * Expected value.
 * E(x) = 1/N sum (x_i, i, N)
 */
static double
calculate_e(struct couple_t *values, get_value_func_t get_value) {
  double e = 0.0;
  int i;
  for (i = 0; i < kNrOfCouples; i++)
    e += get_value(values, i);
  e *= 1.0 / static_cast<double>(kNrOfCouples);
  return e;
}

/*
 * Variance.
 * D(x) = 1/N sum ( (x_i - E(x))^2 , i, N)
 */
static double
calculate_d(double e, struct couple_t *values, get_value_func_t get_value) {
  double d = 0.0;
  int i;
  for (i = 0; i < kNrOfCouples; i++) {
    double tmp = get_value(values, i) - e;
    d += tmp * tmp;
  }
  d *= 1.0 / static_cast<double>(kNrOfCouples);
  return d;
}

/*
 * Covariance.
 * cov(x,y) = 1/N sum ( (x_i - E(x)) * (y_i - E(y)) , i, N)
 */
static double
calculate_cov(double e_x, double e_y, struct couple_t *values) {
  double con_xy = 0.0;
  int i;
  for (i = 0; i < kNrOfCouples; i++) {
    double tmp_x = get_pixel(values, i) - e_x;
    double tmp_y = get_adjacent_pixel(values, i) - e_y;
    con_xy += tmp_x * tmp_y;
  }
  con_xy *= 1.0 / static_cast<double>(kNrOfCouples);
  return con_xy;
}

/*
 * Correlation property.
 * r_(xy) = con(x,y) / (sqrt( D(x) ) * sqrt( D(y) ))
 */
static double
calculate_r(struct couple_t *values) {
  double e_x = calculate_e(values, &get_pixel);
  double e_y = calculate_e(values, &get_adjacent_pixel);
  double d_x = calculate_d(e_x, values, &get_pixel);
  double d_y = calculate_d(e_y, values, &get_adjacent_pixel);
  double cov_xy = calculate_cov(e_x, e_y, values);
  return cov_xy / (sqrt(d_x) * sqrt(d_y));
}

/*
 * Sets the values of the given struct couples *values.
 * Selects kNrOfCouples random pixels and selects depending on the
 * DIMENSION parameter an adjacent pixel.
 */
static void
set_random_values(cimg_library::CImg<unsigned int> image,
    struct couple_t *values, int dimension) {
  unsigned int seed = time(NULL);

  int i, offset_x, offset_y;
  int x, y;
  for (i = 0; i < kNrOfCouples; i++) {
    x = rand_r(&seed) % image.width();
    y = rand_r(&seed) % image.height();

    offset_x = ((rand_r(&seed) % 2) == 0) ? -1 : 1;
    offset_y = ((rand_r(&seed) % 2) == 0) ? -1 : 1;

    if ((x + offset_x) < 0 ||  (x + offset_x) >= image.width())
      offset_x *= -1;

    if ((y + offset_y) < 0 ||  (y + offset_y) >= image.width())
      offset_y *= -1;

    values[i].pixel = image(x, y);

    switch (dimension) {
      case kDimHorizontal:
        values[i].adjacent_pixel = image(x + offset_x, y);
        break;
      case kDimVertical:
        values[i].adjacent_pixel = image(x, y + offset_y);
        break;
      case kDimDiagonal:
        values[i].adjacent_pixel = image(x + offset_x, y + offset_y);
        break;
      default:
        printf("ERROR: invalid dimension! \n");
    }
  }
}

/*
 * Returns the value of the pixel at the position i of the given values.
 */
static int
get_pixel(struct couple_t *values, int i) {
  return values[i].pixel;
}

/*
 * Returns the value of the adjacent pixel at the position i 
 * of the given values.
 */
static int
get_adjacent_pixel(struct couple_t *values, int i) {
  return values[i].adjacent_pixel;
}

/*
 * Writes values to data file.
 */
static void
write_values_2_file(struct couple_t *values, std::string filename) {
  std::ofstream data_file(filename);
  if (data_file.is_open()) {
    for (int i = 0; i < kNrOfCouples; i++) {
      data_file << get_pixel(values, i) << " "
                << get_adjacent_pixel(values, i) << "\n";
    }
  } else {
    printf("data could not be opened.");
  }
  data_file.close();
}
