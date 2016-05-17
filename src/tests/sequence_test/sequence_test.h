// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef SEQT_H_
#define SEQT_H_

#include <stdint.h>
#include <cmath>
#include "../../seth.h"

/* max. value of a pixel is 255 -> 8 bit */
const int kMaxBitsPerPixel = 8;

struct single_bit_t {
  unsigned int number_of_0;
  unsigned int number_of_1;
};

struct double_bit_t {
  unsigned int number_of_00;
  unsigned int number_of_01;
  unsigned int number_of_10;
  unsigned int number_of_11;
};

double run_test_seqt_single(struct img_info_t *img_info);
double run_test_seqt_double(struct img_info_t *img_info);

#endif  // SEQT_H_
