// Copyright (c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef XOR_OTP_CSTD_H_
#define XOR_OTP_CSTD_H_

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <bitset>
#include "../../seth.h"

const int kKeyLengthXorOtpCstd = 1;

void run_enc_xor_otp_cstd_msb(struct img_info_t *img_info);
void run_dec_xor_otp_cstd_msb(struct img_info_t *img_info);
void run_enc_xor_otp_cstd_pixel(struct img_info_t *img_info);
void run_dec_xor_otp_cstd_pixel(struct img_info_t *img_info);

void set_key_xor_otp_cstd(struct img_info_t *img_info);
void set_key_modified_xor_otp_cstd(struct img_info_t *img_info);

#endif  // XOR_OTP_CSTD_H_
