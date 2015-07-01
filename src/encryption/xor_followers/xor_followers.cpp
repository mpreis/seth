// Copyright (c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./xor_followers.h"
#include "../../libraries/CImg.h"

static int kKeyLengthXorFollowers;
static const int kMinKeyLength = 8;

static bool get_bit(unsigned char byte, int position);
static cimg_library::CImg<unsigned int>
encrypt(cimg_library::CImg<unsigned int> origin, int key[], int key_length);
static cimg_library::CImg<unsigned int>
decrypt(cimg_library::CImg<unsigned int> origin, int key[], int key_length);
static void
run_enc_xor_followers(struct img_info_t *img_info, int key_length);
static void
run_dec_xor_followers(struct img_info_t *img_info, int key_length);
static void
set_key_modified_xor_followers(struct img_info_t *img_info, int key_length);

void
run_enc_xor_followers_8(struct img_info_t *img_info) {
  run_enc_xor_followers(img_info, 8);
}
void
run_dec_xor_followers_8(struct img_info_t *img_info) {
  run_dec_xor_followers(img_info, 8);
}
void
run_enc_xor_followers_32(struct img_info_t *img_info) {
  run_enc_xor_followers(img_info, 32);
}
void
run_dec_xor_followers_32(struct img_info_t *img_info) {
  run_dec_xor_followers(img_info, 32);
}
void
run_enc_xor_followers_256(struct img_info_t *img_info) {
  run_enc_xor_followers(img_info, 256);
}
void
run_dec_xor_followers_256(struct img_info_t *img_info) {
  run_dec_xor_followers(img_info, 256);
}

static void
run_enc_xor_followers(struct img_info_t *img_info, int key_length) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted
    = encrypt(origin, img_info->key, key_length);
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_xor_followers(struct img_info_t *img_info, int key_length) {
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted
    = decrypt(origin, img_info->key, key_length);
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_xor_followers(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  kKeyLengthXorFollowers
    = (rand_r(&seed) % (kMaxKeyLength - kMinKeyLength)) + kMinKeyLength;

  img_info->key_length = kKeyLengthXorFollowers;
  for (int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = (i < kKeyLengthXorFollowers)
      ? (rand_r(&seed) % 2)
        ? 0
        : 1
      : kKeyEmpty;
}

void
set_key_modified_xor_followers_8(struct img_info_t *img_info) {
  set_key_modified_xor_followers(img_info, 8);
}
void
set_key_modified_xor_followers_32(struct img_info_t *img_info) {
  set_key_modified_xor_followers(img_info, 32);
}
void
set_key_modified_xor_followers_256(struct img_info_t *img_info) {
  set_key_modified_xor_followers(img_info, 256);
}

static void
set_key_modified_xor_followers(struct img_info_t *img_info, int key_length) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % key_length;
  img_info->key[i] = (img_info->key[i] == 0) ? 1 : 0;
}

static cimg_library::CImg<unsigned int>
encrypt(cimg_library::CImg<unsigned int> origin, int key[], int key_length) {
  cimg_library::CImg<unsigned int> encrypted(origin);
  int new_bit = 0, act_bit_pos = 0,
    act_pos_x = 0, act_pos_y = 0,
    img_width = origin.width(), img_height = origin.height();

  int * following_bits;
  following_bits = new int[key_length];

  int x, y;
  cimg_forXY(encrypted, x, y) {
    for (int bit_pos = kGsBits-1; bit_pos >= 0; bit_pos--) {
      act_bit_pos = bit_pos;
      act_pos_x = x;
      act_pos_y = y;

      // get the next k bits from the image
      for (int j = key_length-1; j >= 0; j--) {
        if (act_bit_pos > 0) {
          act_bit_pos--;
        } else {
          act_bit_pos = kGsBits-1;
          if (act_pos_x < (img_width-1)) {
            act_pos_x++;
          } else {
            act_pos_x = 0;
            act_pos_y = (act_pos_y + 1) % img_height;
          }
        }
        following_bits[j]
          = get_bit(encrypted(act_pos_x, act_pos_y, 0, 0), act_bit_pos);
      }

      // xor the following bits with the key
      for (int j = key_length-1; j >= 0; j--) {
        following_bits[j] = following_bits[j] ^ key[j];
      }

      // xor each bit of the following bits
      new_bit = following_bits[key_length] ^ following_bits[key_length-1];
      for (int j = key_length-2; j >= 0; j--) {
        new_bit = new_bit ^ following_bits[j];
      }

      // set new bit in the encrypted image
      encrypted(x, y, 0, 0)
        = (encrypted(x, y, 0, 0) ^ (new_bit << bit_pos)) & 255;
    }
  }
  return encrypted;
}

static cimg_library::CImg<unsigned int>
decrypt(cimg_library::CImg<unsigned int> origin, int key[], int key_length) {
  cimg_library::CImg<unsigned int> decrypted(origin);
  int new_bit = 0, act_bit_pos = 0,
    act_pos_x = 0, act_pos_y = 0,
    img_width = origin.width(), img_height = origin.height();

  int * following_bits;
  following_bits = new int[key_length];

  int x, y;
  cimg_forXY(decrypted, x, y) {
    for (int bit_pos = 0; bit_pos <= kGsBits-1; bit_pos++) {
      act_bit_pos = bit_pos;
      act_pos_x = (origin.width() - 1) - x;
      act_pos_y = (origin.height() - 1) - y;

      // get the next k bits from the image
      for (int j = key_length-1; j >= 0; j--) {
        if (act_bit_pos > 0) {
          act_bit_pos--;
        } else {
          act_bit_pos = kGsBits-1;
          if (act_pos_x < (img_width-1)) {
            act_pos_x++;
          } else {
            act_pos_x = 0;
            act_pos_y = (act_pos_y + 1) % img_height;
          }
        }

        following_bits[j]
          = get_bit(decrypted(act_pos_x, act_pos_y, 0, 0), act_bit_pos);
      }

      // xor the following bits with the key
      for (int j = key_length-1; j >= 0; j--) {
        following_bits[j] = following_bits[j] ^ key[j];
      }

      // xor each bit of the following bits
      new_bit = following_bits[key_length] ^ following_bits[key_length-1];
      for (int j = key_length-2; j >= 0; j--) {
        new_bit = new_bit ^ following_bits[j];
      }

      // set new bit in the decrypted image
      decrypted((origin.width() - 1) - x, (origin.height() - 1) - y, 0, 0)
        = (decrypted((origin.width() - 1) - x,
          (origin.height() - 1) - y, 0, 0) ^ (new_bit << bit_pos)) & 255;
    }
  }
  return decrypted;
}

/*
 * position in range 0-7
 */
static bool
get_bit(unsigned char byte, int position) {
    return (byte >> position) & 0x1;
}
