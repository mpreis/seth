// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef ENCRYPTION_ARNOLDS_CAT_MAP_H_
#define ENCRYPTION_ARNOLDS_CAT_MAP_H_

#include <iostream>
#include <fstream>
#include <cmath>

#include "../../seth.h"

const int kMaxKeyValueArnold = 10;  // randomly selected
const int kMaxIterArnold = 45;
const int kMinIterArnold = 10;
const int kKeyLengthArnold = 2;
const int kKeyIdxP = 0;
const int kKeyIdxQ = 1;
const int kKeyIdxNrOfIters = 2;

void run_enc_arnold(struct img_info_t *img_info);
void run_dec_arnold(struct img_info_t *img_info);

void set_key_arnold(struct img_info_t *img_info);
void set_key_modified_arnold(struct img_info_t *img_info);

#endif  // ENCRYPTION_ARNOLDS_CAT_MAP_H_
