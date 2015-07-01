// Copyright (c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include <bitset>
#include <cmath>
#include <string>
#include "./xor_otp_cstd.h"
#include "../../libraries/CImg.h"

// Function prototypes
bool getBit(unsigned char byte, int position);

cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin, int key);

cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin, int key);


void
run_enc_xor_otp_cstd_msb(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted
    = encrypt_msb(origin, img_info->key[0]);
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_xor_otp_cstd_msb(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted
    = encrypt_msb(origin, img_info->key[0]);
  decrypted.save(img_info->decrypted.c_str());
}

void
run_enc_xor_otp_cstd_pixel(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted
    = encrypt_pixel(origin, img_info->key[0]);
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_xor_otp_cstd_pixel(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted
    = encrypt_pixel(origin, img_info->key[0]);
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_xor_otp_cstd(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  img_info->key_length = kKeyLengthXorOtpCstd;
  for (int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = (i < kKeyLengthXorOtpCstd)
    ? rand_r(&seed)
    : kKeyEmpty;
}

void
set_key_modified_xor_otp_cstd(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kKeyLengthXorOtpCstd;
  int n = rand_r(&seed) % kKeyBits;
  img_info->key[i] ^= (1 << n);
}

cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin, int key) {
  cimg_library::CImg<unsigned int> encrypted(origin);
  int bit_flip = 0;

  unsigned int seed = key;

  for (int s = kGsBits; s >= 0; s--) {
    cimg_forXY(encrypted, x, y) {
      bit_flip = rand_r(&seed) % 2;  // false (0) or true (1)
      if (bit_flip == 1) {      // flip bit
        // flip bit s in pixel x/y of encrypted
        encrypted(x, y, 0, 0) ^= 1 << s;
        encrypted(x, y, 0, 0) &= 255;    // bound it to 8 bit
      }
    }
  }

  return encrypted;
}

cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin, int key) {
  cimg_library::CImg<unsigned int> encrypted(origin);
  int bit_flip = 0;

  unsigned int seed = key;

  cimg_forXY(encrypted, x, y) {
    for (int s = kGsBits; s >= 0; s--) {
      bit_flip = rand_r(&seed) % 2;  // false (0) or true (1)
      if (bit_flip == 1) {      // flip bit
        // flip bit s in pixel x/y of encrypted
        encrypted(x, y, 0, 0) ^= 1 << s;
        encrypted(x, y, 0, 0) &= 255;  // bound it to 8 bit
      }
    }
  }

  return encrypted;
}
