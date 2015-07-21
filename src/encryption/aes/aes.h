// Copyright (c) 2015,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef ENCRYPTION_AES_H_
#define ENCRYPTION_AES_H_

#include "../../seth.h"

//
// ECB
//
void run_enc_aes_ecb_128(struct img_info_t *img_info);
void run_dec_aes_ecb_128(struct img_info_t *img_info);

void run_enc_aes_ecb_192(struct img_info_t *img_info);
void run_dec_aes_ecb_192(struct img_info_t *img_info);

void run_enc_aes_ecb_256(struct img_info_t *img_info);
void run_dec_aes_ecb_256(struct img_info_t *img_info);

//
// CBC
//
void run_enc_aes_cbc_128(struct img_info_t *img_info);
void run_dec_aes_cbc_128(struct img_info_t *img_info);

void run_enc_aes_cbc_192(struct img_info_t *img_info);
void run_dec_aes_cbc_192(struct img_info_t *img_info);

void run_enc_aes_cbc_256(struct img_info_t *img_info);
void run_dec_aes_cbc_256(struct img_info_t *img_info);

//
// CFB
//
void run_enc_aes_cfb_128(struct img_info_t *img_info);
void run_dec_aes_cfb_128(struct img_info_t *img_info);

void run_enc_aes_cfb_192(struct img_info_t *img_info);
void run_dec_aes_cfb_192(struct img_info_t *img_info);

void run_enc_aes_cfb_256(struct img_info_t *img_info);
void run_dec_aes_cfb_256(struct img_info_t *img_info);

//
// Key utilities
//
void set_key_aes(struct img_info_t *img_info);
void set_key_modified_aes(struct img_info_t *img_info);

#endif  // ENCRYPTION_AES_H_
