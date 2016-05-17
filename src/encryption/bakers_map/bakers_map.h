// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef BAKERS_MAP_H_
#define BAKERS_MAP_H_

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "../../seth.h"

const int kMinIterBaker = 10;  // randomly selected
const int kMaxIterBaker = 45;  // randomly selected

void run_enc_baker(struct img_info_t *img_info);
void run_dec_baker(struct img_info_t *img_info);
void run_enc_baker_sub(struct img_info_t *img_info);
void run_dec_baker_sub(struct img_info_t *img_info);

void set_key_baker(struct img_info_t *img_info);
void set_key_modified_baker(struct img_info_t *img_info);

#endif  // BAKERS_MAP_H_
