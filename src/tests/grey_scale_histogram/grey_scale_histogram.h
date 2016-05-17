// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#ifndef GRSH_H_
#define GRSH_H_

#include <string>
#include "../../seth.h"

const std::string kGrshDataFileExtension = ".dat";
const std::string kGrshDataFileName = "grsh_values" + kGrshDataFileExtension;

double run_test_grsh(struct img_info_t *img_info);

#endif  // GRSH_H_
