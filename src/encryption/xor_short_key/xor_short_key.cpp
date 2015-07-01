// Copyright (c) 2015, the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./xor_short_key.h"
#include "../../libraries/CImg.h"

static const int kMaxKeyLengthInt = 8;
static const int kIntLength = 32;

static int
get_n_th_bit(int pixel, int n);

static cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin,
  int key[], int key_length);

static cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin,
  int key[], int key_length);

static void
setup(struct img_info_t *img_info, int key[], int key_length);
static void
run_enc_xor_short_key_msb(struct img_info_t *img_info, int key_length);
static void
run_dec_xor_short_key_msb(struct img_info_t *img_info, int key_length);
static void
run_enc_xor_short_key_pixel(struct img_info_t *img_info, int key_length);
static void
run_dec_xor_short_key_pixel(struct img_info_t *img_info, int key_length);

void
run_enc_xor_short_key_msb_8(struct img_info_t *img_info) {
  run_enc_xor_short_key_msb(img_info, 8);
}
void
run_dec_xor_short_key_msb_8(struct img_info_t *img_info) {
  run_dec_xor_short_key_msb(img_info, 8);
}
void
run_enc_xor_short_key_msb_32(struct img_info_t *img_info) {
  run_enc_xor_short_key_msb(img_info, 32);
}
void
run_dec_xor_short_key_msb_32(struct img_info_t *img_info) {
  run_dec_xor_short_key_msb(img_info, 32);
}
void
run_enc_xor_short_key_msb_256(struct img_info_t *img_info) {
  run_enc_xor_short_key_msb(img_info, 256);
}
void
run_dec_xor_short_key_msb_256(struct img_info_t *img_info) {
  run_dec_xor_short_key_msb(img_info, 256);
}

void
run_enc_xor_short_key_pixel_8(struct img_info_t *img_info) {
  run_enc_xor_short_key_pixel(img_info, 8);
}
void
run_dec_xor_short_key_pixel_8(struct img_info_t *img_info) {
  run_dec_xor_short_key_pixel(img_info, 8);
}
void
run_enc_xor_short_key_pixel_32(struct img_info_t *img_info) {
  run_enc_xor_short_key_pixel(img_info, 32);
}
void
run_dec_xor_short_key_pixel_32(struct img_info_t *img_info) {
  run_dec_xor_short_key_pixel(img_info, 32);
}
void
run_enc_xor_short_key_pixel_256(struct img_info_t *img_info) {
  run_enc_xor_short_key_pixel(img_info, 256);
}
void
run_dec_xor_short_key_pixel_256(struct img_info_t *img_info) {
  run_dec_xor_short_key_pixel(img_info, 256);
}

static void
run_enc_xor_short_key_msb(struct img_info_t *img_info, int key_length) {
  int key[kMaxKeyLengthInt];
  setup(img_info, key, key_length);
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted
    = encrypt_msb(origin, key, key_length);
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_xor_short_key_msb(struct img_info_t *img_info, int key_length) {
  int key[kMaxKeyLengthInt];
  setup(img_info, key, key_length);
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted
    = encrypt_msb(origin, key, key_length);
  decrypted.save(img_info->decrypted.c_str());
}

static void
run_enc_xor_short_key_pixel(struct img_info_t *img_info, int key_length) {
  int key[kMaxKeyLengthInt];
  setup(img_info, key, key_length);
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted
    = encrypt_pixel(origin, key, key_length);
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_xor_short_key_pixel(struct img_info_t *img_info, int key_length) {
  int key[kMaxKeyLengthInt];
  setup(img_info, key, key_length);
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted
    = encrypt_pixel(origin, key, key_length);
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_xor_short(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  img_info->key_length = kKeyLengthXorShort;
  for (int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = (i < kKeyLengthXorShort) ? rand_r(&seed) : kKeyEmpty;
}

void
set_key_modified_xor_short(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kKeyLengthXorShort;
  int n = rand_r(&seed) % kKeyBits;
  img_info->key[i] ^= (1 << n);
}

static void
setup(struct img_info_t *img_info, int key[], int key_length) {
  unsigned int seed = img_info->key[0];
  for (int i = 0; i < ceil(static_cast<double>(key_length) / kIntLength); i++)
    key[i] = rand_r(&seed);
}


static cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin,
    int key[], int key_length) {

  cimg_library::CImg<unsigned int> encrypted(origin);
  int x, y, current_bit, xor_mask;
  int key_pos = 0, key_bit_pos = 0;
  int len = (key_length < 32) ? key_length : 32;

  int i = 0;
  cimg_forXY(encrypted, x, y) {
    for (int j = 0; j < kMsb; j++) {
      if (key_length > 32 && key_bit_pos == len-1)
        key_pos += 1;

      if (key_pos == (key_length / 32))
        key_pos = 0;

      key_bit_pos = (i % len);

      current_bit = get_n_th_bit(key[key_pos], key_bit_pos);
      xor_mask = current_bit << (kLsb + j);

      encrypted(x, y) = encrypted(x, y) ^ xor_mask;
      i++;
    }
  }
  return encrypted;
}

static cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin,
    int key[], int key_length) {

  cimg_library::CImg<unsigned int> encrypted(origin);

  int i, x, y, current_bit, xor_mask;
  int key_pos = 0, key_bit_pos = 0;
  int len = (key_length < 32) ? key_length : 32;

  for (i = 0; i < kMsb; i++) {
    if (key_length > 32 && key_bit_pos == len-1)
      key_pos += 1;

    if (key_pos == (key_length / 32))
      key_pos = 0;

    current_bit = get_n_th_bit(key[key_pos], key_bit_pos);
    xor_mask = current_bit << (kMsb - i);

    cimg_forXY(encrypted, x, y) {
      encrypted(x, y) = (encrypted(x, y) ^ xor_mask) % 256;
    }
  }
  return encrypted;
}

static int
get_n_th_bit(int pixel, int n) {
  return (pixel & ( 1 << n )) >> n;
}
