// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./sequence_test.h"
#include "../../libraries/CImg.h"

static struct single_bit_t
single_bit_test(cimg_library::CImg<unsigned int> image);

static struct double_bit_t
double_bit_test(cimg_library::CImg<unsigned int> image);

static int get_number_of_set_bits(uint8_t i);
static double_bit_t get_double_bit_value_of_pixel(uint8_t pixel);

/*
 * Single Bit
 *
 * CHI-QUADRAT-TEST: Verteilungstest
 * - Erwartungswert: 0
 */
double
run_test_seqt_single(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());
  struct single_bit_t s_result = single_bit_test(image);

  unsigned int nr_of_pixels = image.width() * image.height();
  unsigned int total_number_of_bits = nr_of_pixels * kMaxBitsPerPixel;

  double p = 0.5;
  double n = p * total_number_of_bits;
  double chi_square_single
    = pow((s_result.number_of_0 - n), 2) / n
    + pow((s_result.number_of_1 - n), 2) / n;

  img_info->results[TEST_SEQT_S_0]
    = static_cast<double>(s_result.number_of_0)
    / static_cast<double>(total_number_of_bits);

  img_info->results[TEST_SEQT_S_1]
    = static_cast<double>(s_result.number_of_1)
    / static_cast<double>(total_number_of_bits);

  return chi_square_single;
}

/*
 * Double Bit
 *
 * CHI-QUADRAT-TEST: Verteilungstest
 * - Erwartungswert: 0
 */
double
run_test_seqt_double(struct img_info_t *img_info) {
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());
  struct double_bit_t d_result = double_bit_test(image);

  unsigned int nr_of_pixels = (image.width() * image.height()) / 2;
  unsigned int total_number_of_bits = nr_of_pixels * kMaxBitsPerPixel;

  double p = 0.25;
  double n = p * total_number_of_bits;
  double chi_square_double
    = pow((d_result.number_of_00 - n), 2) / n
    + pow((d_result.number_of_01 - n), 2) / n
    + pow((d_result.number_of_10 - n), 2) / n
    + pow((d_result.number_of_11 - n), 2) / n;

  img_info->results[TEST_SEQT_D_00]
    = static_cast<double>(d_result.number_of_00)
    / static_cast<double>(total_number_of_bits);
  img_info->results[TEST_SEQT_D_01]
    = static_cast<double>(d_result.number_of_01)
    / static_cast<double>(total_number_of_bits);
  img_info->results[TEST_SEQT_D_10]
    = static_cast<double>(d_result.number_of_10)
    / static_cast<double>(total_number_of_bits);
  img_info->results[TEST_SEQT_D_11]
    = static_cast<double>(d_result.number_of_11)
    / static_cast<double>(total_number_of_bits);

  return chi_square_double;
}

/*
 * Single bit test.
 * Returns the number of set bits.
 */
static struct single_bit_t
single_bit_test(cimg_library::CImg<unsigned int> image) {
  unsigned int number_of_pixels = image.width() * image.height();
  unsigned int total_number_of_bits = number_of_pixels * kMaxBitsPerPixel;
  int number_of_set_bits = 0;

  // scan each pixel and count the set bits
  cimg_forXY(image, x, y) {
    uint8_t pixel = image(x, y);
    int set_bits = get_number_of_set_bits(pixel);
    if (set_bits >= 0 && set_bits <= kMaxBitsPerPixel) {
      number_of_set_bits += set_bits;
    } else {
      printf("ERROR: invalid number of set bits (%d)!\n", set_bits);
      break;
    }
  }

  struct single_bit_t result;
  result.number_of_1 = number_of_set_bits;
  result.number_of_0 = total_number_of_bits - number_of_set_bits;
  return result;
}

/*
 * Double bit test.
 */
static struct double_bit_t
double_bit_test(cimg_library::CImg<unsigned int> image) {
  struct double_bit_t result;
  result.number_of_00 = 0;
  result.number_of_01 = 0;
  result.number_of_10 = 0;
  result.number_of_11 = 0;

  cimg_forXY(image, x, y) {
      uint8_t pixel = image(x, y);
      struct double_bit_t tmp = get_double_bit_value_of_pixel(pixel);
      result.number_of_00 += tmp.number_of_00;
      result.number_of_01 += tmp.number_of_01;
      result.number_of_10 += tmp.number_of_10;
      result.number_of_11 += tmp.number_of_11;
  }
  return result;
}

/*
 * Returns the number of set bits (Hamming Weight), i.e.,
 * 8 bit representing the number 7:
 * 0000 0111 -> Hamming Weight = 3
 *
 */
static int
get_number_of_set_bits(uint8_t i) {
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


/*
 * Counts the double bits of a single pixel.
 */
static double_bit_t
get_double_bit_value_of_pixel(uint8_t pixel) {
  struct double_bit_t result;
  result.number_of_00 = 0;
  result.number_of_01 = 0;
  result.number_of_10 = 0;
  result.number_of_11 = 0;

  for (int i = 0; i < (kMaxBitsPerPixel-1); i++) {
      int i_th_bit = (pixel & (1 << i)) >> i;
      i++;
      int i_plus_one_th_bit = (pixel & (1 << i)) >> i;

      if (i_th_bit) {
        if (i_plus_one_th_bit)
          result.number_of_11++;
        else
          result.number_of_10++;
      } else {
        if (i_plus_one_th_bit)
          result.number_of_01++;
        else
          result.number_of_00++;
      }
  }
  return result;
}
