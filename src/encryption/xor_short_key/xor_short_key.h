// Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef XOR_SHORT_KEY_H_
#define XOR_SHORT_KEY_H_

#include <math.h>
#include <iostream>
#include <fstream>
#include "../../seth.h"

const int kMsb = 7;   // most significant bit
const int kLsb = 0;   // least significant bit

const int kKeyLengthXorShort = 1;

void run_enc_xor_short_key_msb_8(struct img_info_t *img_info);
void run_dec_xor_short_key_msb_8(struct img_info_t *img_info);
void run_enc_xor_short_key_msb_32(struct img_info_t *img_info);
void run_dec_xor_short_key_msb_32(struct img_info_t *img_info);
void run_enc_xor_short_key_msb_256(struct img_info_t *img_info);
void run_dec_xor_short_key_msb_256(struct img_info_t *img_info);

void run_enc_xor_short_key_pixel_8(struct img_info_t *img_info);
void run_dec_xor_short_key_pixel_8(struct img_info_t *img_info);
void run_enc_xor_short_key_pixel_32(struct img_info_t *img_info);
void run_dec_xor_short_key_pixel_32(struct img_info_t *img_info);
void run_enc_xor_short_key_pixel_256(struct img_info_t *img_info);
void run_dec_xor_short_key_pixel_256(struct img_info_t *img_info);

void set_key_xor_short(struct img_info_t *img_info);
void set_key_modified_xor_short(struct img_info_t *img_info);

#endif  // XOR_SHORT_KEY_H_
