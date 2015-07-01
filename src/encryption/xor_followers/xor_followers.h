// Copyright(c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef XOR_FOLLOWERS_H_
#define XOR_FOLLOWERS_H_

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <bitset>
#include "../../seth.h"

void run_enc_xor_followers_8(struct img_info_t *img_info);
void run_dec_xor_followers_8(struct img_info_t *img_info);
void run_enc_xor_followers_32(struct img_info_t *img_info);
void run_dec_xor_followers_32(struct img_info_t *img_info);
void run_enc_xor_followers_256(struct img_info_t *img_info);
void run_dec_xor_followers_256(struct img_info_t *img_info);

void set_key_xor_followers(struct img_info_t *img_info);
void set_key_modified_xor_followers_8(struct img_info_t *img_info);
void set_key_modified_xor_followers_32(struct img_info_t *img_info);
void set_key_modified_xor_followers_256(struct img_info_t *img_info);

#endif  // XOR_FOLLOWERS_H_
