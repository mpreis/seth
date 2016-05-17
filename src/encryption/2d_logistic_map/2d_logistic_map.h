// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef ENCRYPTION_2D_LOSISTIC_MAP_H_
#define ENCRYPTION_2D_LOSISTIC_MAP_H_

#include <cmath>
#include <vector>

#include "../../seth.h"

// denotes the number of intensity scales
const int F = 256;

// S equals 4, because our plaintext image is a 8-bit grayscale image
const int kS = 4;

const int kLENC[kS][kS] = {
  {4, 2, 1, 3},
  {1, 3, 4, 2},
  {2, 4, 3, 1},
  {3, 1, 2, 4}};

const int kLDEC[kS][kS] = {
  {71, 216, 173, 117},
  {173, 117, 71, 216},
  {216, 71, 117, 173},
  {117, 173, 216, 71}};

const int kGN = 0;
const int kGR = 1;
const int kGS = 2;
const int kGD = 3;

const int kGX[kS][kS] = {
  {kGN, kGR, kGS, kGD},
  {kGR, kGS, kGD, kGN},
  {kGS, kGD, kGN, kGR},
  {kGD, kGN, kGR, kGS}};

void run_enc_2d_logistic_map(struct img_info_t *img_info);
void run_dec_2d_logistic_map(struct img_info_t *img_info);

void set_key_2d_logistic_map(struct img_info_t *img_info);
void set_key_modified_2d_logistic_map(struct img_info_t *img_info);

#endif
