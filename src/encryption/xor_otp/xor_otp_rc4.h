// Copyright (c) 2016,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef XOR_OTP_RC4_H_
#define XOR_OTP_RC4_H_

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <bitset>
#include "../../seth.h"

const int kMaxKeyValueXorOtpRc4 = 256;
const int kKeyLengthXorOtpRc4 = 256;

void run_enc_xor_otp_rc4_msb(struct img_info_t *img_info);
void run_dec_xor_otp_rc4_msb(struct img_info_t *img_info);
void run_enc_xor_otp_rc4_pixel(struct img_info_t *img_info);
void run_dec_xor_otp_rc4_pixel(struct img_info_t *img_info);
void set_key_xor_otp_rc4(struct img_info_t *img_info);
void set_key_modified_xor_otp_rc4(struct img_info_t *img_info);

#endif  // XOR_OTP_RC4_H_
