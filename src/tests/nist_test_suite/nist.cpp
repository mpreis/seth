// Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./nist.h"
#include "../../libraries/CImg.h"

extern "C" {
  #include "./include/decls.h"
  #include "./include/utilities.h"
}

double
run_test_nist(struct img_info_t *img_info)
{
  cimg_library::CImg<unsigned int> image(img_info->encrypted.c_str());

  tp.n = image.width() * image.height() * 8;
  tp.blockFrequencyBlockLength = 128;
  tp.nonOverlappingTemplateBlockLength = 9;
  tp.overlappingTemplateBlockLength = 9;
  tp.approximateEntropyBlockLength = 10;
  tp.serialBlockLength = 16;
  tp.linearComplexitySequenceLength = 500;
  tp.numOfBitStreams = 1;

  std::string path = img_info->outdir + "nist/key_" + std::to_string((long long int)(img_info->ith_key))
                     + "/" + img_info->enc_mode + "/";

  for(int i = 1; i<=NUMOFTESTS; i++)
  {
    testVector[i] = 1;

    std::string testName(testNames[i]);
    std::string outputpath = path + testName + "/";
    std::string cmd = "mkdir -p " + outputpath;
    system(cmd.c_str());

    std::string statsfile = outputpath + "stats.txt";
    std::string resultsfile = outputpath + "results.txt";

    stats[i] = fopen(statsfile.c_str(), "w");
    results[i] = fopen(resultsfile.c_str(), "w");
  }

  epsilon = (BitSequence *) calloc(tp.n, sizeof(BitSequence));
  int idx = 0;
  cimg_forXY(image, x, y) {
    char pixel = image(x,y);
    epsilon[idx++] = ((pixel & 128) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &  64) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &  32) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &  16) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &   8) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &   4) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &   2) == 0) ? 0 : 1;
    epsilon[idx++] = ((pixel &   1) == 0) ? 0 : 1;
  }

  nist_test_suite();

  for(int i = 1; i<=NUMOFTESTS; i++)
  {
    fclose(stats[i]);
    fclose(results[i]);
  }

  return 0.0;
}
