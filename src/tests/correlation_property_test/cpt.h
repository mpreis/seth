// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef CPT_H_
#define CPT_H_

#include <string>
#include "../../seth.h"

const int kNrOfCouples = 5000;
const int kDimHorizontal = 0;
const int kDimVertical = 1;
const int kDimDiagonal = 2;

const std::string kCprtDataFileExtension = ".dat";
const std::string kCprtDataFileNameV = "cpt_v_values" + kCprtDataFileExtension;
const std::string kCprtDataFileNameH = "cpt_h_values" + kCprtDataFileExtension;
const std::string kCprtDataFileNameD = "cpt_d_values" + kCprtDataFileExtension;

struct couple_t {
  int pixel;
  int adjacent_pixel;
};

typedef int (*get_value_func_t)(struct couple_t *, int);

double run_test_cprt_vertical(struct img_info_t *img_info);
double run_test_cprt_horizontal(struct img_info_t *img_info);
double run_test_cprt_diagonal(struct img_info_t *img_info);

#endif  // CPT_H_
