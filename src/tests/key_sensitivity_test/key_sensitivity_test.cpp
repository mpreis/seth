// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./key_sensitivity_test.h"
#include "../../libraries/CImg.h"
#include "../../tests/npcr/npcr.h"

double
run_test_kese(struct img_info_t *img_info) {
  struct img_info_t tmp = {};
  tmp.key_length = img_info->key_length;

  // get key
  img_info->set_key_modified_func(img_info);
  for (int i=0; i < img_info->key_length; i++)
    tmp.key[i] = img_info->key[i];

  // decrypt encrypted image with modified key
  tmp.encrypted = img_info->encrypted;
  tmp.decrypted = img_info->decrypted;
  img_info->dec_func(&tmp);

  // compare origin and decrypted image
  tmp.origin = img_info->origin;
  tmp.encrypted = tmp.decrypted;
  return run_test_npcr (&tmp);
}
