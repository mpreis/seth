// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./arnolds_cat_map.h"
#include "../../libraries/CImg.h"

void
run_enc_arnold(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);

  int p = img_info->key[kKeyIdxP];
  int q = img_info->key[kKeyIdxQ];

  // arnolds cat map enc
  for (int r = 0; r < img_info->key[kKeyIdxNrOfIters]; r++) {
    cimg_forXY(encrypted, x, y) {
      encrypted(x, y) = origin(
        (x + (p * y)) % origin.width(),
        ((q * x) + (p * q * y) + y) % origin.height());
    }
    origin = encrypted;
  }
  // save encrypted image
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_arnold(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);
  int p = img_info->key[kKeyIdxP];
  int q = img_info->key[kKeyIdxQ];

  // arnolds cat map dec
  for (int r = 0; r < img_info->key[kKeyIdxNrOfIters]; r++) {
    cimg_forXY(encrypted, x, y) {
      decrypted(
        (x + (p*y)) % decrypted.width(),
        ((q*x) + (p*q*y) + y) % decrypted.height()) = encrypted(x, y);
    }
    encrypted = decrypted;
  }
  // save decrypted image
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_arnold(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  img_info->key_length = kKeyLengthArnold;
  for (int i = 0; i < kMaxKeyLength; i++) {
    img_info->key[i] = (i < kKeyLengthArnold)
      ? (rand_r(&seed) % (kMaxKeyValueArnold - 1)) + 1
      :kKeyEmpty;
  }
  img_info->key[kKeyIdxNrOfIters] =
    (rand_r(&seed) % (kMaxIterArnold - kMinIterArnold)) + kMinIterArnold;
}

void
set_key_modified_arnold(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kKeyLengthArnold;
  int n = rand_r(&seed) % kKeyBits;
  img_info->key[i] ^= (1 << n);
}
