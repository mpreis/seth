// Copyright (c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include <algorithm>
#include "./xor_otp_rc4.h"
#include "../../libraries/CImg.h"

// Function prototypes
static bool get_bit(unsigned char byte, int position);
static void initialize(u_char *State);
static void swap(u_char *a, u_char *b);
static void ksa(u_char *State, u_char *key, int key_length);
static u_char * prng(u_char *State, int msglength);

static cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin,
  const u_char* keystream);

static cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin, const u_char* keystream);

static u_char*
get_keystream(int number_of_pixels, int xor_otp_rc4_key[], int key_length);

void
run_enc_xor_otp_rc4_msb(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  u_char *keystream = get_keystream(
    origin.width() * origin.height(),
    img_info->key,
    img_info->key_length);
  cimg_library::CImg<unsigned int> encrypted = encrypt_msb(origin, keystream);
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_xor_otp_rc4_msb(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  u_char *keystream = get_keystream(
    origin.width() * origin.height(),
    img_info->key,
    img_info->key_length);
  cimg_library::CImg<unsigned int> decrypted = encrypt_msb(origin, keystream);
  decrypted.save(img_info->decrypted.c_str());
}

void
run_enc_xor_otp_rc4_pixel(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  u_char *keystream = get_keystream(
    origin.width() * origin.height(),
    img_info->key,
    img_info->key_length);
  cimg_library::CImg<unsigned int> encrypted
    = encrypt_pixel(origin, keystream);
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_xor_otp_rc4_pixel(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->encrypted.c_str());
  u_char *keystream = get_keystream(
    origin.width() * origin.height(),
    img_info->key,
    img_info->key_length);
  cimg_library::CImg<unsigned int> decrypted
    = encrypt_pixel(origin, keystream);
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_xor_otp_rc4(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  img_info->key_length = kKeyLengthXorOtpRc4;
  for (int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = (i < kKeyLengthXorOtpRc4)
      ? rand_r(&seed) % kMaxKeyValueXorOtpRc4
      : kKeyEmpty;
}

void
set_key_modified_xor_otp_rc4(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kKeyLengthXorOtpRc4;
  int n = rand_r(&seed) % kKeyBits;
  img_info->key[i] ^= (1 << n);
}

static u_char *
get_keystream(int number_of_pixels, int xor_otp_rc4_key[], int key_length) {
  u_char State[kKeyLengthXorOtpRc4];
  u_char key[kKeyLengthXorOtpRc4];    // The encryption/decryption key

  // set first pixel as key seed and compute keystream
  for (int i = 0; i < kKeyLengthXorOtpRc4; i++)
    key[i] = (u_char) xor_otp_rc4_key[i];

  initialize(State);
  ksa(State, key, key_length);
  return prng(State, number_of_pixels);
}

static cimg_library::CImg<unsigned int>
encrypt_msb(cimg_library::CImg<unsigned int> origin, const u_char* keystream) {
  cimg_library::CImg<unsigned int> encrypted(origin);
  int i = 0, gs_bit = 0;

  for (int s = kGsBits; s >= 0; s--) {    // go from msb to lsb
    i = 0;
    cimg_forXY(encrypted, x, y) {      // for each pixel
      gs_bit =
        (encrypted(x, y, 0, 0) ^ (get_bit(keystream[i/kGsBits], s) << (2^s)))
        & 255;
      encrypted(x, y, 0, 0) = gs_bit;
      i++;
    }
  }
  return encrypted;
}

static cimg_library::CImg<unsigned int>
encrypt_pixel(cimg_library::CImg<unsigned int> origin,
    const u_char* keystream) {
  cimg_library::CImg<unsigned int> encrypted(origin);
  int i = 0;

  cimg_forXY(encrypted, x, y) {
    encrypted(x, y, 0, 0) = (encrypted(x, y, 0, 0) ^ keystream[i]) & 255;
    i++;
  }
  return encrypted;
}

// position in range 0-7
static bool
get_bit(unsigned char byte, int position) {
  return (byte >> position) & 0x1;
}

/*
 * RC4 functions
 */

/* Initialize State[256] to the identity permutation. */
static void
initialize(u_char *State) {
  int i;

  for (i = 0; i < 256; i++) {
    State[i] = i;
  }
  return;
}

/* Swap array elements i=State[i] and b=State[j]. */
static void
swap(u_char *i, u_char *j) {
  u_char temp;
  temp = *i;
  *i = *j;
  *j = temp;
}

/* Key scheduling algorithm. Swap array elements based on the key. */
static void
ksa(u_char *State, u_char *key, int key_length) {
  int j = 0;
  for (int i = 0; i < 256; i++) {
    j = (j + State[i] + key[i%key_length]) % 256;
    swap(&State[i], &State[j]);
  }
}

/* Pseudo-random number generator: Generate the keystream. */
static u_char *
prng(u_char *State, int msglength) {
  int i = 0, j = 0, k;

  u_char *keystream;

  keystream = reinterpret_cast<u_char *>(malloc(sizeof(u_char)*msglength));

  for (k = 0; k < msglength; k++) {
    i = (i+1) % 256;
    j = (j+State[i]) % 256;

    swap(&State[i], &State[j]);

    keystream[k] = State[(State[i]+State[j]) % 256];
  }
  return keystream;
}
