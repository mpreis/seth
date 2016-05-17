# Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is governed
# by a BSD license that can be found in the LICENSE file.

import os
import csv
import sys
import json
import math

FAILURE = 'FAILURE'
SUCCESS = 'SUCCESS'

def prepare(path):
  samples = dict()
  quotes = dict()

  for (_, out_dirnames, _) in os.walk(path):
    for img_dir in out_dirnames:
      nist_path = path + img_dir + '/nist'
      
      for (_, key_dirnames, _) in os.walk(nist_path):
        for key_dir in key_dirnames:
          key_path = nist_path + '/' + key_dir

          for (_, enc_dirnames, _) in os.walk(key_path):
            for enc_dir in enc_dirnames:
              enc_path = key_path + '/' + enc_dir
              try:
                values = samples[enc_dir]
              except KeyError:
                samples[enc_dir] = dict()

              try:
                values = quotes[enc_dir]
              except KeyError:
                quotes[enc_dir] = dict()

              for (_, test_dirnames, _) in os.walk(enc_path):
                for test_dir in test_dirnames:
                  test_path = enc_path + '/' + test_dir

                  try:
                    values = samples[enc_dir][test_dir]
                  except KeyError:
                    samples[enc_dir][test_dir] = list()

                  with open(test_path + '/results.txt') as fp:
                    for line in fp:
                      samples[enc_dir][test_dir].append(float(line))



                  try:
                    values = quotes[enc_dir][test_dir]
                  except KeyError:
                    quotes[enc_dir][test_dir] = dict()
                    quotes[enc_dir][test_dir][SUCCESS] = 0
                    quotes[enc_dir][test_dir][FAILURE] = 0

                  with open(test_path + '/stats.txt') as fp:
                    for line in fp:
                      if SUCCESS in line:
                        quotes[enc_dir][test_dir][SUCCESS] += 1
                      elif FAILURE in line:
                        quotes[enc_dir][test_dir][FAILURE] += 1
                break;
            break;
        break;
    break;



  with open(path + 'nist_results.csv', 'w+') as results_out:
    writer = csv.writer(results_out)
    for enc in sorted(samples):
      for test in sorted(samples[enc]):
        row = (enc, test, ) + tuple(samples[enc][test])
        writer.writerow(row)

  with open(path + 'nist_results_quotes.csv', 'w+') as quotes_out:
    writer = csv.writer(quotes_out)
    writer.writerow(('encryption', 'test', 'success', 'total', 'quote'))
    for enc in sorted(quotes):
      for test in sorted(quotes[enc]):
        try:
          total = quotes[enc][test][SUCCESS] + quotes[enc][test][FAILURE]
          if total == 0:
            quote = 0
          else:
            quote = quotes[enc][test][SUCCESS] / total
          row = (enc, test, quotes[enc][test][SUCCESS], total, quote)
          writer.writerow(row)
        except KeyError:
          continue;

  print("DONE.")

def get_arith_mean(numbers):
  if len(numbers) < 1:
    return 0;
  s = float(0)
  for number in numbers:
    s = s + float(number)
  return s / len(numbers)

#unbiased sample standard deviation
def get_std_dev(numbers, arith_mean=None):
  if arith_mean is None:
    arith_mean = get_arith_mean(numbers)

  if len(numbers) < 2:
    print("numbers too short")
    return 0;

  ss = float(0)
  for number in numbers:
    ss = ss + math.pow((float(number) - arith_mean), 2)

  return math.sqrt( (1.0/float(len(numbers)-1)) * ss )



if __name__=='__main__':
  print ("\n\tusage: python prepare_nist_data.py path/to/nist_data")
  print ("\tusage: path/to/nist_data is default out/")
  print ("\tusage: expected folder-structure: out/image_name_xy/nist/nist_test_xy\n")

  if len(sys.argv) == 2:
    prepare(sys.argv[1])