// Copyright (c) 2015,   the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include <vector>

#include "./bakers_map.h"
#include "../../libraries/CImg.h"

static int kKeyLengthBaker;
static int kNrOfItersBaker;

void
run_enc_baker(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);

  float N = origin.width();
  std::vector<float> n(kKeyLengthBaker+1);

  // calc n
  // erg: float n[5] = {0, 128, 192, 448, 512};
  for (int i = 1; i <= kKeyLengthBaker; i++)
    n[i] = n[i-1] + static_cast<float>(img_info->key[i-1]);

  // bakers map enc
  for (int r = 0; r < kNrOfItersBaker; r++) {
    cimg_forXY(encrypted, x, y) {
      for (int i = 0; i < kKeyLengthBaker; i++) {
        if (n[i] <= x && x < n[i+1]) {
          float k_i = static_cast<float>(img_info->key[i]);
          encrypted(x, y) = origin(
            static_cast<int>((N/k_i) * (x-n[i]))
            + (y % static_cast<int>(N/k_i)),
            static_cast<int>((k_i/N)
            * (y - (y % static_cast<int>(N/k_i)))) + n[i]);
          break;
        }
      }
    }
    origin = encrypted;
  }
  // save encrypted image
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_baker(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  float N = encrypted.width();
  std::vector<float> n(kKeyLengthBaker+1);

  // calc n
  // erg: float n[5] = {0, 128, 192, 448, 512};
  for (int i = 1; i <= kKeyLengthBaker; i++)
    n[i] = n[i-1] + static_cast<float>(img_info->key[i-1]);

  // bakers map dec
  for (int r = 0; r < kNrOfItersBaker; r++) {
    cimg_forXY(decrypted, x, y) {
      for (int i = 0; i < kKeyLengthBaker; i++) {
        if (n[i] <= x && x < n[i+1]) {
          float k_i = static_cast<float>(img_info->key[i]);
          decrypted(
            static_cast<int>((N/k_i) * (x-n[i]))
            + (y % static_cast<int>(N/k_i)),
            static_cast<int>((k_i/N)
            * (y - (y % static_cast<int>(N/k_i)))) + n[i])
            = encrypted(x, y);
          break;
        }
      }
    }
    encrypted = decrypted;
  }
  // save decrypted image
  decrypted.save(img_info->decrypted.c_str());
}

void
run_enc_baker_sub(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);

  float N = origin.width();
  std::vector<float> n(kKeyLengthBaker+1);

  // calc n
  // erg: float n[5] = {0, 128, 192, 448, 512};
  for (int i = 1; i <= kKeyLengthBaker; i++)
    n[i] = n[i-1] + static_cast<float>(img_info->key[i-1]);

  // bakers map enc
  for (int r = 0; r < kNrOfItersBaker; r++) {
    cimg_forXY(encrypted, x, y) {
      for (int i = 0; i < kKeyLengthBaker; i++) {
        if (n[i] <= x && x < n[i+1]) {
          float k_i = static_cast<float>(img_info->key[i]);
          encrypted(x, y) = origin(
            static_cast<int>((N/k_i) * (x-n[i]))
            + (y % static_cast<int>(N/k_i)),
            static_cast<int>((k_i/N)
            * (y - (y % static_cast<int>(N/k_i)))) + n[i]);

          encrypted(x,  y,  0,  0)  = (
                static_cast<int>(static_cast<int>((N/k_i) * (x-n[i]))
                + (y % static_cast<int>(N/k_i))
                * static_cast<int>((k_i/N)
                * (y - (y % static_cast<int>(N/k_i)))) + n[i]
                + origin(
                  static_cast<int>((N/k_i) * (x-n[i]))
                  + (y % static_cast<int>(N/k_i)),
                  static_cast<int>((k_i/N)
                  * (y - (y % static_cast<int>(N/k_i)))) + n[i],  0,  0)))
              % kTonalRange;
          break;
        }
      }
    }
    origin = encrypted;
  }
  // save encrypted image
  encrypted.save(img_info->encrypted.c_str());
}

void
run_dec_baker_sub(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  float N = encrypted.width();
  std::vector<float> n(kKeyLengthBaker+1);

  // calc n
  // erg: float n[5] = {0, 128, 192, 448, 512};
  for (int i = 1; i <= kKeyLengthBaker; i++)
    n[i] = n[i-1] + static_cast<float>(img_info->key[i-1]);

  // bakers map dec
  for (int r = 0; r < kNrOfItersBaker; r++) {
    cimg_forXY(decrypted, x, y) {
      for (int i = 0; i < kKeyLengthBaker; i++) {
        if (n[i] <= x && x < n[i+1]) {
          float k_i = static_cast<float>(img_info->key[i]);
          decrypted(
            static_cast<int>((N/k_i) * (x-n[i]))
            + (y % static_cast<int>(N/k_i)),
            static_cast<int>((k_i/N)
            * (y - (y % static_cast<int>(N/k_i)))) + n[i]) = encrypted(x, y);

          decrypted(
            static_cast<int>((N/k_i) * (x-n[i]))
            + (y % static_cast<int>(N/k_i)),
            static_cast<int>((k_i/N)
            * (y - (y % static_cast<int>(N/k_i)))) + n[i],  0,  0 )
          = (encrypted(x,  y,  0,  0) -
              static_cast<int>(static_cast<int>((N/k_i) * (x-n[i]))
              + (y % static_cast<int>(N/k_i)) *
              static_cast<int>((k_i/N)
              * (y - (y % static_cast<int>(N/k_i)))) + n[i])) % kTonalRange;
          break;
        }
      }
    }
    encrypted = decrypted;
  }
  // save decrypted image
  decrypted.save(img_info->decrypted.c_str());
}

void
set_key_baker(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  cimg_library::CImg<unsigned int> img(img_info->origin.c_str());
  int kImg_width = img.width();
  int tmp[kImg_width];
  int i,  sum = 0;
  for (i = 0; sum < kImg_width; i++) {
    int r = (rand_r(&seed) % (kImg_width / 2)) + 2;
    if ((sum + r) > kImg_width)
      r = kImg_width - sum;
    sum += r;
    tmp[i] = r;
  }

  kKeyLengthBaker = i;
  img_info->key_length = kKeyLengthBaker;
  for (int j = 0; j < kMaxKeyLength; j++)
    img_info->key[j] = (j < kKeyLengthBaker) ? tmp[j] : kKeyEmpty;

  kNrOfItersBaker =
    (rand_r(&seed) % (kMaxIterBaker - kMinIterBaker)) + kMinIterBaker;
}

void
set_key_modified_baker(struct img_info_t *img_info) {
  unsigned int seed = time(NULL);
  int i = rand_r(&seed) % kKeyLengthBaker;
    img_info->key[i]--;
    i = ((i+1) < kKeyLengthBaker) ? (i+1) : 0;
    img_info->key[i]++;
}
