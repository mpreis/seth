// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./aes.h"
#include "../../libraries/CImg.h"
#include "../../libraries/cryptopp562/aes.h"
#include "../../libraries/cryptopp562/cryptlib.h"
#include "../../libraries/cryptopp562/modes.h"
#include "../../libraries/cryptopp562/osrng.h"

static void run_enc_aes_cfb(struct img_info_t *img_info, int keylength);
static void run_dec_aes_cfb(struct img_info_t *img_info, int keylength);

static void run_enc_aes_cbc(struct img_info_t *img_info, int keylength);
static void run_dec_aes_cbc(struct img_info_t *img_info, int keylength);

static void run_enc_aes_ecb(struct img_info_t *img_info, int keylength);
static void run_dec_aes_ecb(struct img_info_t *img_info, int keylength);

CryptoPP::AutoSeededRandomPool rnd;
CryptoPP::SecByteBlock key;
byte iv[CryptoPP::AES::BLOCKSIZE];

/* IMPORTANT NOTES:
    AES was only added to evaluate the performance.
    However, most of the implement evaluation test will still work.
    There is only one exception: Key Sensitivity Test. This test
    won't work.
*/

//
// ECB
//
void
run_enc_aes_ecb_128(struct img_info_t *img_info) {
  run_enc_aes_ecb(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_dec_aes_ecb_128(struct img_info_t *img_info) {
  run_dec_aes_ecb(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_enc_aes_ecb_192(struct img_info_t *img_info) {
  run_enc_aes_ecb(img_info, 24);
}
void
run_dec_aes_ecb_192(struct img_info_t *img_info) {
  run_dec_aes_ecb(img_info, 24);
}
void
run_enc_aes_ecb_256(struct img_info_t *img_info) {
  run_enc_aes_ecb(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}
void
run_dec_aes_ecb_256(struct img_info_t *img_info) {
  run_dec_aes_ecb(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}


//
// CBC
//
void
run_enc_aes_cbc_128(struct img_info_t *img_info) {
  run_enc_aes_cbc(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_dec_aes_cbc_128(struct img_info_t *img_info) {
  run_dec_aes_cbc(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_enc_aes_cbc_192(struct img_info_t *img_info) {
  run_enc_aes_cbc(img_info, 24);
}
void
run_dec_aes_cbc_192(struct img_info_t *img_info) {
  run_dec_aes_cbc(img_info, 24);
}
void
run_enc_aes_cbc_256(struct img_info_t *img_info) {
  run_enc_aes_cbc(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}
void
run_dec_aes_cbc_256(struct img_info_t *img_info) {
  run_dec_aes_cbc(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}


//
// CFB
//
void
run_enc_aes_cfb_128(struct img_info_t *img_info) {
  run_enc_aes_cfb(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_dec_aes_cfb_128(struct img_info_t *img_info) {
  run_dec_aes_cfb(img_info, CryptoPP::AES::MIN_KEYLENGTH);
}
void
run_enc_aes_cfb_192(struct img_info_t *img_info) {
  run_enc_aes_cfb(img_info, 24);
}
void
run_dec_aes_cfb_192(struct img_info_t *img_info) {
  run_dec_aes_cfb(img_info, 24);
}
void
run_enc_aes_cfb_256(struct img_info_t *img_info) {
  run_enc_aes_cfb(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}
void
run_dec_aes_cfb_256(struct img_info_t *img_info) {
  run_dec_aes_cfb(img_info, CryptoPP::AES::MAX_KEYLENGTH);
}


//
// AES utilities
//
static void
run_enc_aes_cfb(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);

  CryptoPP::SecByteBlock tmp_key(0x00, keylength);
  key = tmp_key;
  rnd.GenerateBlock(key, key.size());
  rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

  int i = 0;
  int kLen = origin.width() * origin.height();
  byte plainText[kLen];
  byte cipherText[kLen];
  cimg_forXY(origin, x, y) {
    plainText[i] = static_cast<byte>(origin(x, y));
    i++;
  }

  CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfb_enc(key, key.size(), iv);
  cfb_enc.ProcessData(cipherText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    encrypted(x, y) = static_cast<int>(cipherText[i]);
    i++;
  }
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_aes_cfb(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  int i = 0;
  int kLen = encrypted.width() * encrypted.height();
  byte plainText[kLen];
  cimg_forXY(encrypted, x, y) {
    plainText[i] = static_cast<byte>(encrypted(x, y));
    i++;
  }

  CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfb_dec(key, key.size(), iv);
  cfb_dec.ProcessData(plainText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    decrypted(x, y) = static_cast<int>(plainText[i]);
    i++;
  }
  decrypted.save(img_info->decrypted.c_str());
}

static void
run_enc_aes_cbc(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);


  CryptoPP::SecByteBlock tmp_key(0x00, keylength);
  key = tmp_key;
  rnd.GenerateBlock(key, key.size());
  rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

  int i = 0;
  int kLen = origin.width() * origin.height();
  byte plainText[kLen];
  byte cipherText[kLen];
  cimg_forXY(origin, x, y) {
    plainText[i] = static_cast<byte>(origin(x, y));
    i++;
  }

  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption cbc_enc(key, key.size(), iv);
  cbc_enc.ProcessData(cipherText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    encrypted(x, y) = static_cast<int>(cipherText[i]);
    i++;
  }
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_aes_cbc(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  int i = 0;
  int kLen = encrypted.width() * encrypted.height();
  byte plainText[kLen];
  cimg_forXY(encrypted, x, y) {
    plainText[i] = static_cast<byte>(encrypted(x, y));
    i++;
  }

  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption cbc_dec(key, key.size(), iv);
  cbc_dec.ProcessData(plainText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    decrypted(x, y) = static_cast<int>(plainText[i]);
    i++;
  }
  decrypted.save(img_info->decrypted.c_str());
}

static void
run_enc_aes_ecb(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> origin(img_info->origin.c_str());
  cimg_library::CImg<unsigned int> encrypted(origin);


  CryptoPP::SecByteBlock tmp_key(0x00, keylength);
  key = tmp_key;
  rnd.GenerateBlock(key, key.size());
  rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

  int i = 0;
  int kLen = origin.width() * origin.height();
  byte plainText[kLen];
  byte cipherText[kLen];
  cimg_forXY(origin, x, y) {
    plainText[i] = static_cast<byte>(origin(x, y));
    i++;
  }

  CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption ecb_enc(key, key.size());
  ecb_enc.ProcessData(cipherText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    encrypted(x, y) = static_cast<int>(cipherText[i]);
    i++;
  }
  encrypted.save(img_info->encrypted.c_str());
}

static void
run_dec_aes_ecb(struct img_info_t *img_info, int keylength) {
  cimg_library::CImg<unsigned int> encrypted(img_info->encrypted.c_str());
  cimg_library::CImg<unsigned int> decrypted(encrypted);

  int i = 0;
  int kLen = encrypted.width() * encrypted.height();
  byte plainText[kLen];
  cimg_forXY(encrypted, x, y) {
    plainText[i] = static_cast<byte>(encrypted(x, y));
    i++;
  }

  CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption ecb_dec(key, key.size());
  ecb_dec.ProcessData(plainText, plainText, kLen);

  i = 0;
  cimg_forXY(encrypted, x, y) {
    decrypted(x, y) = static_cast<int>(plainText[i]);
    i++;
  }
  decrypted.save(img_info->decrypted.c_str());
}

//
// Key utilities
//
void
set_key_aes(struct img_info_t *img_info) {}
void
set_key_modified_aes(struct img_info_t *img_info) {}
