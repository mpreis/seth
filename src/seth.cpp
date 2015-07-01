// Copyright (c) 2015, the Seth Project Authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is
// governed  by a BSD license that can be found in the LICENSE file.

#include "./seth.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <map>

static void init();
static int run_experiment();
/* utilities: command-line arguments */
static int init_args(int argc, char* argv[]);
static void get_arg_arguments(std::list<std::string> *input_args, int argc,
                              char* argv[], int *optind);
static char * get_one_arg_argument(int argc, char* argv[], int *optind);
static void print_cmd_line_help(void);
/* utitlities: time */
static const std::string current_date_time();
/* utilities: math */
static double get_average(std::list<double> *l);
static double get_variance(std::list<double> *l, double avg);
/* functions */
static std::map<std::string, test_func_t> test_funcs;
static std::map<std::string, func_t> enc_funcs;
static std::map<std::string, func_t> dec_funcs;
static std::map<std::string, func_t> set_key_funcs;
static std::map<std::string, func_t> set_key_modified_funcs;
/* input values */
static std::list<std::string> args_encs;
static std::list<std::string> args_imgs;
static std::list<std::string> args_tests;
static uint32_t kNrOfKeys;
static bool kLessOutputFiles;
/* origin method: needed to run tests without encryption */
void run_enc_origin(struct img_info_t *img_info) {}
void run_dec_origin(struct img_info_t *img_info) {}

void
set_key_xor_origin(struct img_info_t *img_info) {
  img_info->key_length = kMaxKeyLength;
  for (int i = 0; i < kMaxKeyLength; i++)
    img_info->key[i] = kKeyEmpty;
}
void
set_key_modified_origin(struct img_info_t *img_info) {
  set_key_xor_origin(img_info);
}


/*
 * Initializes and runs experiment.
 */
int main(int argc, char* argv[]) {
  init();
  if (init_args(argc, argv) == EXIT_FAILURE)
    return EXIT_FAILURE;
  run_experiment();
  return EXIT_SUCCESS;
}

/*
 * initializes function arrays and creates output directory.
 */
static void
init() {
  /* encryption */
  enc_funcs[ENC_ORIGIN] = run_enc_origin;
  enc_funcs[ENC_ARNOLD] = run_enc_arnold;
  enc_funcs[ENC_BAKER] = run_enc_baker;
  enc_funcs[ENC_BAKER_SUB] = run_enc_baker_sub;
  enc_funcs[ENC_XOR_OTP_RC4_MSB] = run_enc_xor_otp_rc4_msb;
  enc_funcs[ENC_XOR_OTP_RC4_PIX] = run_enc_xor_otp_rc4_pixel;
  enc_funcs[ENC_XOR_OTP_CSTD_MSB] = run_enc_xor_otp_cstd_msb;
  enc_funcs[ENC_XOR_OTP_CSTD_PIX] = run_enc_xor_otp_cstd_pixel;
  enc_funcs[ENC_XOR_KEY_MSB_8] = run_enc_xor_short_key_msb_8;
  enc_funcs[ENC_XOR_KEY_PIX_8] = run_enc_xor_short_key_pixel_8;
  enc_funcs[ENC_XOR_KEY_MSB_32] = run_enc_xor_short_key_msb_32;
  enc_funcs[ENC_XOR_KEY_PIX_32] = run_enc_xor_short_key_pixel_32;
  enc_funcs[ENC_XOR_KEY_MSB_256] = run_enc_xor_short_key_msb_256;
  enc_funcs[ENC_XOR_KEY_PIX_256] = run_enc_xor_short_key_pixel_256;
  enc_funcs[ENC_XOR_FOLLOWERS_8] = run_enc_xor_followers_8;
  enc_funcs[ENC_XOR_FOLLOWERS_32] = run_enc_xor_followers_32;
  enc_funcs[ENC_XOR_FOLLOWERS_256] = run_enc_xor_followers_256;
  /* decryption */
  dec_funcs[ENC_ORIGIN] = run_dec_origin;
  dec_funcs[ENC_ARNOLD] = run_dec_arnold;
  dec_funcs[ENC_BAKER] = run_dec_baker;
  dec_funcs[ENC_BAKER_SUB] = run_dec_baker_sub;
  dec_funcs[ENC_XOR_OTP_RC4_MSB] = run_dec_xor_otp_rc4_msb;
  dec_funcs[ENC_XOR_OTP_RC4_PIX] = run_dec_xor_otp_rc4_pixel;
  dec_funcs[ENC_XOR_OTP_CSTD_MSB] = run_dec_xor_otp_cstd_msb;
  dec_funcs[ENC_XOR_OTP_CSTD_PIX] = run_dec_xor_otp_cstd_pixel;
  dec_funcs[ENC_XOR_KEY_MSB_8] = run_dec_xor_short_key_msb_8;
  dec_funcs[ENC_XOR_KEY_PIX_8] = run_dec_xor_short_key_pixel_8;
  dec_funcs[ENC_XOR_KEY_MSB_32] = run_dec_xor_short_key_msb_32;
  dec_funcs[ENC_XOR_KEY_PIX_32] = run_dec_xor_short_key_pixel_32;
  dec_funcs[ENC_XOR_KEY_MSB_256] = run_dec_xor_short_key_msb_256;
  dec_funcs[ENC_XOR_KEY_PIX_256] = run_dec_xor_short_key_pixel_256;
  dec_funcs[ENC_XOR_FOLLOWERS_8] = run_dec_xor_followers_8;
  dec_funcs[ENC_XOR_FOLLOWERS_32] = run_dec_xor_followers_32;
  dec_funcs[ENC_XOR_FOLLOWERS_256] = run_dec_xor_followers_256;
  /* set key function */
  set_key_funcs[ENC_ORIGIN] = set_key_xor_origin;
  set_key_funcs[ENC_ARNOLD] = set_key_arnold;
  set_key_funcs[ENC_BAKER] = set_key_baker;
  set_key_funcs[ENC_BAKER_SUB] = set_key_baker;
  set_key_funcs[ENC_XOR_OTP_RC4_MSB] = set_key_xor_otp_rc4;
  set_key_funcs[ENC_XOR_OTP_RC4_PIX] = set_key_xor_otp_rc4;
  set_key_funcs[ENC_XOR_KEY_MSB_8] = set_key_xor_short;
  set_key_funcs[ENC_XOR_KEY_PIX_8] = set_key_xor_short;
  set_key_funcs[ENC_XOR_KEY_MSB_32] = set_key_xor_short;
  set_key_funcs[ENC_XOR_KEY_PIX_32] = set_key_xor_short;
  set_key_funcs[ENC_XOR_KEY_MSB_256] = set_key_xor_short;
  set_key_funcs[ENC_XOR_KEY_PIX_256] = set_key_xor_short;
  set_key_funcs[ENC_XOR_FOLLOWERS_8] = set_key_xor_followers;
  set_key_funcs[ENC_XOR_FOLLOWERS_32] = set_key_xor_followers;
  set_key_funcs[ENC_XOR_FOLLOWERS_256] = set_key_xor_followers;
  set_key_funcs[ENC_XOR_OTP_CSTD_MSB] = set_key_xor_otp_cstd;
  set_key_funcs[ENC_XOR_OTP_CSTD_PIX] = set_key_xor_otp_cstd;
  /* set key modified function */
  set_key_modified_funcs[ENC_ORIGIN] = set_key_modified_origin;
  set_key_modified_funcs[ENC_ARNOLD] = set_key_modified_arnold;
  set_key_modified_funcs[ENC_BAKER] = set_key_modified_baker;
  set_key_modified_funcs[ENC_BAKER_SUB] = set_key_modified_baker;
  set_key_modified_funcs[ENC_XOR_OTP_RC4_MSB] = set_key_modified_xor_otp_rc4;
  set_key_modified_funcs[ENC_XOR_OTP_RC4_PIX] = set_key_modified_xor_otp_rc4;
  set_key_modified_funcs[ENC_XOR_OTP_CSTD_MSB] = set_key_modified_xor_otp_cstd;
  set_key_modified_funcs[ENC_XOR_OTP_CSTD_PIX] = set_key_modified_xor_otp_cstd;
  set_key_modified_funcs[ENC_XOR_KEY_MSB_8] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_KEY_PIX_8] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_KEY_MSB_32] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_KEY_PIX_32] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_KEY_MSB_256] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_KEY_PIX_256] = set_key_modified_xor_short;
  set_key_modified_funcs[ENC_XOR_FOLLOWERS_8] =
    set_key_modified_xor_followers_8;
  set_key_modified_funcs[ENC_XOR_FOLLOWERS_32] =
    set_key_modified_xor_followers_32;
  set_key_modified_funcs[ENC_XOR_FOLLOWERS_256] =
    set_key_modified_xor_followers_256;
  /* checks / tests */
  test_funcs[TEST_CPRT_V] = run_test_cprt_vertical;
  test_funcs[TEST_CPRT_H] = run_test_cprt_horizontal;
  test_funcs[TEST_CPRT_D] = run_test_cprt_diagonal;
  test_funcs[TEST_SEQT_S] = run_test_seqt_single;
  test_funcs[TEST_SEQT_D] = run_test_seqt_double;
  test_funcs[TEST_ENTR_I] = run_test_entr_img;
  test_funcs[TEST_ENTR_H] = run_test_entr;
  test_funcs[TEST_NPCR] = run_test_npcr;
  test_funcs[TEST_UACI] = run_test_uaci;
  test_funcs[TEST_KESE] = run_test_kese;
  test_funcs[TEST_GRSV] = run_test_grsv;
  test_funcs[TEST_GRSH] = run_test_grsh;
  // test_funcs[TEST_RGBH] = NULL;
  kNrOfKeys = 1;
  kLessOutputFiles = false;
  mkdir(OUT_DIR.c_str(), kDefaultRights);
}


/*
 *
 */
static int run_experiment() {
  if (args_encs.empty() && !args_tests.empty())
    args_encs.push_front(ENC_ORIGIN);

  if (args_imgs.empty() || (args_encs.empty() && args_tests.empty())) {
    printf("ERROR: invalid combination of arguments (see help).\n");
    return EXIT_FAILURE;
  }

  printf("start-time: %s\n", current_date_time().c_str());

  /* iterate over images */
  std::map<std::string, std::map<std::string, std::list<double> > >
    all_samples;

  for (std::list<std::string>::iterator ii = args_imgs.begin();
      ii != args_imgs.end(); ii++) {
    printf("  %s ... ", (*ii).c_str());
    std::string outdir = OUT_DIR + get_filename(*ii) + "/";
    std::string outdir_imgs = outdir + OUT_DIR_IMGS + "/";
    mkdir(outdir.c_str(), kDefaultRights);
    mkdir(outdir_imgs.c_str(), kDefaultRights);

    /* iterate over number of keys */
    for (uint32_t i = 0; i < kNrOfKeys; i++) {
      bool setup_header = true;
      std::string csv_header("");
      std::string csv_data("");
      /* iterate over encryptions */
      for (std::list<std::string>::iterator ie = args_encs.begin();
          ie != args_encs.end(); ie++) {
        csv_data.append(*ie);
        /* check if given encryption mode is valid. */
        if (enc_funcs.find((std::string)(*ie)) == enc_funcs.end()) {
          printf("ERROR: invalid encryption mode (%s).\n", (*ie).c_str());
          return EXIT_FAILURE;
        }

        // printf("%s ", (*ie).c_str());
        /* build name of encrypted file */
        std::string enc_filename("");
        std::string dec_filename("");
        if (ENC_ORIGIN != (*ie)) {
          enc_filename.append(outdir_imgs);
          enc_filename.append(get_filename(*ii));
          enc_filename.append("_enc_");
          enc_filename.append(*ie);
          enc_filename.append("_" + std::to_string(static_cast<int>(i)));
          enc_filename.append(get_extension(*ii));
          dec_filename.append(outdir_imgs);
          dec_filename.append(get_filename(*ii));
          dec_filename.append("_dec_");
          dec_filename.append(*ie);
          enc_filename.append("_" + std::to_string(static_cast<int>(i)));
          dec_filename.append(get_extension(*ii));
        } else {
          enc_filename.append(*ii);
          dec_filename.append(*ii);
        }

        /* init image info */
        struct img_info_t img_info = {};
        img_info.enc_mode = *ie;

        img_info.outdir = outdir;
        img_info.origin = *ii;
        img_info.encrypted = enc_filename;
        img_info.decrypted = dec_filename;

        img_info.enc_func = enc_funcs[*ie];
        img_info.dec_func = dec_funcs[*ie];
        img_info.set_key_func = set_key_funcs[*ie];
        img_info.set_key_modified_func = set_key_modified_funcs[*ie];

        /* run encryption */
        img_info.set_key_func(&img_info);
        std::chrono::high_resolution_clock::time_point
          t1 = std::chrono::high_resolution_clock::now();
        img_info.enc_func(&img_info);
        std::chrono::high_resolution_clock::time_point
          t2 = std::chrono::high_resolution_clock::now();
        all_samples[*ie][TEST_PERF].push_front(
            std::chrono::duration_cast<std::chrono::milliseconds>
              (t2 - t1).count());

        /* iterate over tests */
        for (std::list<std::string>::iterator it = args_tests.begin();
            it != args_tests.end(); it++) {
          double test_result = test_funcs[*it] (&img_info);
          img_info.results[*it] = test_result;
          all_samples[*ie][*it].push_front(test_result);
          csv_data.append(CSV_VALUE_SEP);
          csv_data.append(std::to_string(static_cast<double>(test_result)));

          if (*it == TEST_SEQT_S) {
            double result_seqts_0 = img_info.results[TEST_SEQT_S_0];
            double result_seqts_1 = img_info.results[TEST_SEQT_S_1];
            all_samples[*ie][TEST_SEQT_S_0].push_front(result_seqts_0);
            all_samples[*ie][TEST_SEQT_S_1].push_front(result_seqts_1);
            // append order has to be in reverse order
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_1)));
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_0)));
          }

          if (*it == TEST_SEQT_D) {
            double result_seqts_00 = img_info.results[TEST_SEQT_D_00];
            double result_seqts_01 = img_info.results[TEST_SEQT_D_01];
            double result_seqts_10 = img_info.results[TEST_SEQT_D_10];
            double result_seqts_11 = img_info.results[TEST_SEQT_D_11];
            all_samples[*ie][TEST_SEQT_D_00].push_front(result_seqts_00);
            all_samples[*ie][TEST_SEQT_D_01].push_front(result_seqts_01);
            all_samples[*ie][TEST_SEQT_D_10].push_front(result_seqts_10);
            all_samples[*ie][TEST_SEQT_D_11].push_front(result_seqts_11);
            // append order has to be in reverse order
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_11)));
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_10)));
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_01)));
            csv_data.append(CSV_VALUE_SEP);
            csv_data.append(
              std::to_string(static_cast<double>(result_seqts_00)));
          }

          if (setup_header) {
            csv_header.append(CSV_VALUE_SEP);
            csv_header.append(*it);
            if (*it == TEST_SEQT_S) {
              // append order has to be in reverse order
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_S_1);
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_S_0);
            }

            if (*it == TEST_SEQT_D) {
              // append order has to be in reverse order
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_D_11);
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_D_10);
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_D_01);
              csv_header.append(CSV_VALUE_SEP);
              csv_header.append(TEST_SEQT_D_00);
            }
          }
        }
        csv_data.append(CSV_LINE_SEP);
        setup_header = false;
      }
      csv_header.append(CSV_LINE_SEP);

      if (!kLessOutputFiles) {
        /* create output file */
        std::ofstream outfile(outdir + get_filename(*ii) +
          "_data_" + std::to_string(static_cast<int>(i)) + "" + CSV_FILE_EXT);
        outfile << csv_header << csv_data;
        outfile.close();
      }
    }

    /* remove created output of this image */
    if (kLessOutputFiles) {
      std::string cmd = "rm -rf " + outdir;
      int sr = system(cmd.c_str());
      printf("cleanup status: %d ... ", sr);
    }
    printf("done.\n");
  }

  /* create output file */
  std::string csv_header_total("");
  std::string csv_data_total("");
  bool setup_header = true;
  for (std::map<std::string,
                std::map<std::string, std::list<double> > >::reverse_iterator
      ienc = all_samples.rbegin();
      ienc != all_samples.rend(); ienc++) {
     csv_data_total.append(ienc->first);
     for (std::map<std::string, std::list<double> >::reverse_iterator
          itest = ienc->second.rbegin();
          itest != ienc->second.rend(); itest++) {
       double avg = get_average(&itest->second);
       double var = get_variance(&itest->second, avg);
      if (setup_header) {
        csv_header_total.append(CSV_VALUE_SEP);
        csv_header_total.append(itest->first);
        csv_header_total.append(CSV_VALUE_SEP);
        csv_header_total.append(LABEL_VARIANCE);
      }
      csv_data_total.append(CSV_VALUE_SEP);
      csv_data_total.append(std::to_string(static_cast<double>(avg)));
      csv_data_total.append(CSV_VALUE_SEP);
      csv_data_total.append(std::to_string(static_cast<double>(var)));
     }
     csv_data_total.append(CSV_LINE_SEP);
     setup_header = false;
  }
  csv_header_total.append(CSV_LINE_SEP);
  std::ofstream total_results_out(OUT_DIR +
    OUT_TOTAL_RESLUTS_FILENAME + CSV_FILE_EXT);
  total_results_out << csv_header_total << csv_data_total;
  total_results_out.close();
  printf("end-time: %s\n", current_date_time().c_str());
  return EXIT_SUCCESS;
}


/******************************************************************************
 * UTILITIES: TIME
 *****************************************************************************/

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
static const std::string
current_date_time() {
    time_t now = time(NULL);
    struct tm tstruct;
    char buf[80];
    localtime_r(&now, &tstruct);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}


/******************************************************************************
 * UTILITIES: MATH
 *****************************************************************************/

static double
get_average(std::list<double> *l) {
  double sum = 0.0;
  for (std::list<double>::iterator iter = l->begin(); iter != l->end(); iter++)
    sum += *iter;
  return sum / l->size();
}

static double
get_variance(std::list<double> *l, double avg) {
  if (l->size() < 2) return 0.0;
  double var = 0.0;
  for (std::list<double>::iterator iter = l->begin(); iter != l->end(); iter++)
    var += pow((*iter - avg), 2);
  return var / static_cast<double>(l->size() - 1);
}


/******************************************************************************
 * UTILITIES: COMANDLINE ARGUMENTS
 *****************************************************************************/

/*
 * Parses the given comandline parameters.
 */
static int
init_args(int argc, char* argv[]) {
  int opt;
  bool input = false;
  char *tmp_arg_dir = NULL;
  char *tmp_arg_key = NULL;
  std::string s;
  if (argc == 1) {
    printf("ERROR: to less arguments (see help). \n");
    return EXIT_FAILURE;
  }

  while ((opt = getopt(argc, argv, "d:t:e:k:lh")) != -1) {
    switch (opt) {
      case 'd':
        if (input) {
          printf("ERROR: to many input files (see help). \n");
          return EXIT_FAILURE;
        }
        tmp_arg_dir = get_one_arg_argument(argc, argv, &optind);
        input = true;
        break;
      case 't':
        get_arg_arguments(&args_tests, argc, argv, &optind);
        break;
      case 'e':
        get_arg_arguments(&args_encs, argc, argv, &optind);
        break;
      case 'k':
        tmp_arg_key = get_one_arg_argument(argc, argv, &optind);
        kNrOfKeys = atoi(tmp_arg_key);
        if (kNrOfKeys < 1) {
          printf("ERROR: number of keys can't be less than 1. \n");
          return EXIT_FAILURE;
        }
        break;
      case 'l':
        kLessOutputFiles = true;
        break;
      case 'h':
        print_cmd_line_help();
      default:
        return EXIT_FAILURE;
    }
  }

  if (tmp_arg_dir != NULL) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(tmp_arg_dir)) != NULL) {
      while ((ent = readdir(dir)) != NULL)
        if (ent->d_type != DT_DIR
          && strcmp(ent->d_name, ".") != 0
          && strcmp(ent->d_name, "..") != 0
          && ent->d_name[0] != '.') {
          std::string _dir(tmp_arg_dir);
          _dir.append(ent->d_name);
          args_imgs.push_front(_dir);
        }
      closedir(dir);
    } else {
      printf("ERROR: could not open directory (%s)\n", tmp_arg_dir);
      return EXIT_FAILURE;
    }
  }

  if (args_imgs.empty()) {
    printf("ERROR: invalid input file. \n");
    return EXIT_FAILURE;
  }

  if (*args_encs.begin() == ARGS_ALL) {
    args_encs.clear();
    for (std::map<std::string, func_t>::iterator it_ef = enc_funcs.begin();
        it_ef != enc_funcs.end(); ++it_ef)
      args_encs.push_front(it_ef->first);
  }

  if (*args_tests.begin() == ARGS_ALL) {
    args_tests.clear();
    for (std::map<std::string, test_func_t>::iterator
        it_tf = test_funcs.begin();
        it_tf != test_funcs.end(); ++it_tf)
      args_tests.push_front(it_tf->first);
  }
  return EXIT_SUCCESS;
}

/*
 * Parses the arguments of an argument which starts with '-'.
 * Sets a std::list of parameters.
 */
static void
get_arg_arguments(std::list<std::string> *input_args, int argc,
                  char* argv[], int *optind) {
  int idx = 0;
  bool all = false;
  (*optind)--;
  for (; (*optind) < argc && *argv[*optind] != '-'; (*optind)++) {
    if (argv[*optind] != NULL && !all) {
      if (argv[*optind] == ARGS_ALL) {
        all = true;
        input_args->clear();
        input_args->push_front(ARGS_ALL);
      } else {
        input_args->push_front(argv[*optind]);
      }
    }
    idx++;
  }
  if (idx == 0) (*optind)++;
}

/*
 * Parses the arguments of an argument which starts with '-'.
 * Returns a single argument (the last one).
 */
static char *
get_one_arg_argument(int argc, char* argv[], int *optind) {
  int idx = 0;
  char *input_arg = NULL;
  (*optind)--;
  for (; (*optind) < argc && *argv[*optind] != '-'; (*optind)++) {
    if (idx < 1) {
      input_arg = argv[*optind];
      idx++;
    }
    if (idx == 0) (*optind)++;
  }
  return input_arg;
}

/*
 * Prints the help.
 */
static void
print_cmd_line_help(void) {
  // print license
  std::cout << std::endl
    << "// Copyright (c) 2015, the Seth Project Authors.  "
    << "All rights reserved." << std::endl
    << "// Please see the AUTHORS file for details.  "
    << "Use of this source code is" << std::endl
    << "// governed  by a BSD license that can be found "
    << "in the LICENSE file." << std::endl << std::endl;

  // print options
  std::cout << "options:" << std::endl
    << "  -h .................. help"
    << std::endl
    << "  -l .................. produce less output files"
    << " (only final result)"
    << std::endl
    << "  -d <directory> ...... directory of source images"
    << std::endl
    << "  -t {<test>} ......... list of tests to executed (min. 1)"
    << std::endl
    << "  -e {<encryption>} ... list of encryptions to executed"
    << " (min. 1)"
    << std::endl
    << "  -k <nr_of_keys> ..... number of random generated keys"
    << " applied on each image (default: 1)"
    << std::endl << std::endl;

  // print tests
  std::cout << "tests:" << std::endl
    << "  " << ARGS_ALL << " ..... runs all tests" << std::endl
    << "  " << TEST_NPCR << " .... NPCR" << std::endl
    << "  " << TEST_UACI << " .... UACI" << std::endl
    << "  " << TEST_GRSH << " .... gray scale histogram" << std::endl
    << "  " << TEST_GRSV << " .... gray scale histogram value" << std::endl
    << "  " << TEST_KESE << " .... key sensitivity" << std::endl
    << "  " << TEST_CPRT_V << " ... correlation property test vertical"
            << std::endl
    << "  " << TEST_CPRT_H << " ... correlation property test horizontal"
            << std::endl
    << "  " << TEST_CPRT_D << " ... correlation property test diagonal"
            << std::endl
    << "  " << TEST_SEQT_S << " ... sequence test single bit" << std::endl
    << "  " << TEST_SEQT_D << " ... sequence test double bit" << std::endl
    << "  " << TEST_ENTR_I << " ... entropy of the image" << std::endl
    << "  " << TEST_ENTR_H << " ... entropy of the gray scale histogram"
    << std::endl << std::endl;

  // print encryptions
  std::cout << "encryptions:" << std::endl
    << "  " << ARGS_ALL << " ................ runs all encryptions"
            << std::endl
    << "  " << ENC_ORIGIN << " ............. no encryption"
            << " (use original image)" << std::endl
    << "  " << ENC_ARNOLD << " ............. Arnold cat's map"
            << std::endl
    << "  " << ENC_BAKER << " .............. Baker's map"
            << std::endl
    << "  " << ENC_BAKER_SUB << " .......... baker's map in substitution mode"
            << std::endl
    << "  " << ENC_XOR_OTP_RC4_MSB << " .... XOR OTP RC4 in MSB mode"
            << std::endl
    << "  " << ENC_XOR_OTP_RC4_PIX << " .... XOR OTP RC4 in pixel mode"
            << std::endl
    << "  " << ENC_XOR_OTP_CSTD_MSB << " ... XOR OTP CSTD in MSB mode"
            << std::endl
    << "  " << ENC_XOR_OTP_CSTD_PIX << " ... XOR OTP CSTD in pixel mode"
            << std::endl
    << "  " << ENC_XOR_KEY_MSB_8 << " ...... XOR short-key in MSB mode"
            << " 8 bit key" << std::endl
    << "  " << ENC_XOR_KEY_PIX_8 << " ...... XOR short-key in pixel mode"
            << " 8 bit key" << std::endl
    << "  " << ENC_XOR_KEY_MSB_32 << " ..... XOR short-key in MSB mode"
            << " 32 bit key" << std::endl
    << "  " << ENC_XOR_KEY_PIX_32 << " ..... XOR short-key in pixel mode"
            << " 32 bit key" << std::endl
    << "  " << ENC_XOR_KEY_MSB_256 << " .... XOR short-key in MSB mode"
            << " 256 bit key" << std::endl
    << "  " << ENC_XOR_KEY_PIX_256 << " .... XOR short-key in pixel mode"
            << "  256 bit key" << std::endl
    << "  " << ENC_XOR_FOLLOWERS_8 << " .... XOR followers 8 bit key"
            << std::endl
    << "  " << ENC_XOR_FOLLOWERS_32 << " ... XOR followers 32 bit key"
            << std::endl
    << "  " << ENC_XOR_FOLLOWERS_256 << " .. XOR followers 256 bit key"
    << std::endl << std::endl;
}

/******************************************************************************
 * UTILITIES: FILE
 *****************************************************************************/

struct match_path_separator {
  bool operator()(char ch) const {
    return ch == '/';
  }
};
struct match_extension_separator {
  bool operator()(char ch) const {
    return ch == '.';
  }
};
std::string
get_filename(const std::string &pathname) {
  return std::string(
    find_if(
      pathname.rbegin(),
      pathname.rend(),
      match_path_separator()).base(),
    find_if(
      pathname.rbegin(),
      pathname.rend(),
      match_extension_separator()).base() - 1);
}

std::string
get_extension(const std::string &pathname) {
  return std::string(
    find_if(
      pathname.rbegin(),
      pathname.rend(),
      match_extension_separator()).base() - 1,
    pathname.end());
}
