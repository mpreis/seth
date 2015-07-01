// Copyright (c) 2015, the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef CHAOS_BASED_ENC_BUSTER_
#define CHAOS_BASED_ENC_BUSTER_

  #include <string>
  #include <map>

  /* tests */
  #include "./tests/correlation_property_test/cpt.h"
  #include "./tests/entropy/entropy.h"
  #include "./tests/npcr/npcr.h"
  #include "./tests/uaci/uaci.h"
  #include "./tests/sequence_test/sequence_test.h"
  #include "./tests/key_sensitivity_test/key_sensitivity_test.h"
  #include "./tests/grey_scale_value/grey_scale_value.h"
  #include "./tests/grey_scale_histogram/grey_scale_histogram.h"

  /* encryptions */
  #include "./encryption/arnolds_cat_map/arnolds_cat_map.h"
  #include "./encryption/bakers_map/bakers_map.h"
  #include "./encryption/xor_otp/xor_otp_rc4.h"
  #include "./encryption/xor_otp/xor_otp_cstd.h"
  #include "./encryption/xor_short_key/xor_short_key.h"
  #include "./encryption/xor_followers/xor_followers.h"

  #define VERSION 0.1
  #define kDefaultRights 0700

  #ifndef cimg_display
    #define cimg_display 0  // disable display
  #endif

  const int kTestDefaultValue = -1.0;
  const int kMaxKeyLength = 256;
  const int kKeyEmpty = 0;
  const int kKeyBits  = 8;
  const int kTonalRange = 256;
  const int kGsBits = 8;

  const std::string OUT_DIR = "out/";
  const std::string OUT_DIR_IMGS = "imgs/";
  const std::string OUT_TOTAL_RESLUTS_FILENAME = "results";

  const std::string ARGS_ALL       = "all";
  const std::string TEST_CPRT_V    = "cprtv";
  const std::string TEST_CPRT_H    = "cprth";
  const std::string TEST_CPRT_D    = "cprtd";
  const std::string TEST_SEQT_S    = "seqts";
  const std::string TEST_SEQT_S_0  = "seqts0";
  const std::string TEST_SEQT_S_1  = "seqts1";
  const std::string TEST_SEQT_D    = "seqtd";
  const std::string TEST_SEQT_D_00 = "seqtd00";
  const std::string TEST_SEQT_D_01 = "seqtd01";
  const std::string TEST_SEQT_D_10 = "seqtd10";
  const std::string TEST_SEQT_D_11 = "seqtd11";
  const std::string TEST_ENTR_I    = "entri";
  const std::string TEST_ENTR_H    = "entrh";
  const std::string TEST_NPCR      = "npcr";
  const std::string TEST_UACI      = "uaci";
  const std::string TEST_GRSV      = "grsv";
  const std::string TEST_KESE      = "kese";
  const std::string TEST_GRSH      = "grsh";
  const std::string TEST_PERF      = "perf";
  const std::string LABEL_VARIANCE = "variance";

  // origin value: needed to run tests without encryption
  const std::string ENC_ORIGIN            = "origin";
  const std::string ENC_ARNOLD            = "arnold";
  const std::string ENC_BAKER             = "baker";
  const std::string ENC_BAKER_SUB         = "baker-sub";
  const std::string ENC_XOR_OTP_RC4_MSB   = "xor-otp-rc4-msb";
  const std::string ENC_XOR_OTP_RC4_PIX   = "xor-otp-rc4-pix";
  const std::string ENC_XOR_OTP_CSTD_PIX  = "xor-otp-cstd-pix";
  const std::string ENC_XOR_OTP_CSTD_MSB  = "xor-otp-cstd-msb";
  const std::string ENC_XOR_KEY_MSB_8     = "xor-key-msb-8";
  const std::string ENC_XOR_KEY_PIX_8     = "xor-key-pix-8";
  const std::string ENC_XOR_KEY_MSB_32    = "xor-key-msb-32";
  const std::string ENC_XOR_KEY_PIX_32    = "xor-key-pix-32";
  const std::string ENC_XOR_KEY_MSB_256   = "xor-key-msb-256";
  const std::string ENC_XOR_KEY_PIX_256   = "xor-key-pix-256";
  const std::string ENC_XOR_FOLLOWERS_8   = "xor-followers-8";
  const std::string ENC_XOR_FOLLOWERS_32  = "xor-followers-32";
  const std::string ENC_XOR_FOLLOWERS_256 = "xor-followers-256";

  const std::string CSV_VALUE_SEP = ";";
  const std::string CSV_LINE_SEP = "\n";
  const std::string CSV_FILE_EXT = ".csv";
  const std::string CSV_HEADER =
    ""          + CSV_VALUE_SEP +
    TEST_CPRT_V + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_CPRT_H + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_CPRT_D + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_SEQT_S + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_SEQT_D + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_ENTR_I + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_ENTR_H + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_NPCR   + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_UACI   + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_KESE   + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_VALUE_SEP +
    TEST_GRSV   + CSV_VALUE_SEP + LABEL_VARIANCE + CSV_LINE_SEP;

  typedef void (*func_t)(struct img_info_t *);
  typedef double (*test_func_t)(struct img_info_t *);
  struct img_info_t {
    std::string outdir;            // output directory
    std::string enc_mode;          // used encryption
    std::string origin;            // path to the origin image
    std::string decrypted;         // path to the decrypted image
    std::string encrypted;         // path to the encrypted image
    func_t enc_func;               // encryption function
    func_t dec_func;               // decryption function
    func_t set_key_func;           // set key function
    func_t set_key_modified_func;  // set modified key function
    int key[kMaxKeyLength];
    int key_length;
    std::map<std::string, double> results;
  };

  std::string get_filename(const std::string &pathname);
  std::string get_extension(const std::string &pathname);

#endif
