# Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is governed
# by a BSD license that can be found in the LICENSE file.

import csv
import sys
import json
import math
from os import listdir
from os.path import isfile, isdir, join

FAILURE = 'FAILURE'
SUCCESS = 'SUCCESS'

RESLUTS = 'results.txt'
STATS = 'stats.txt'

def prepare(path):
  results = dict()
  quotes = dict()

  img_dirs = get_dirs(path)

  for img_dir in img_dirs:
    key_dirs = get_dirs(join(path, img_dir, 'nist'))

    for key_dir in key_dirs:
      enc_dirs = get_dirs(join(path, img_dir, 'nist', key_dir))

      for enc_dir in enc_dirs:
        test_dirs = get_dirs(join(path, img_dir, 'nist', key_dir, enc_dir))

        # initialize if required
        if not enc_dir in results:
          results[enc_dir] = dict()
        if not enc_dir in quotes:
          quotes[enc_dir] = dict()
          
        for test_dir in test_dirs:
          # initialize if required
          if not test_dir in results[enc_dir]:
            results[enc_dir][test_dir] = list()
          if not test_dir in quotes[enc_dir]:
            quotes[enc_dir][test_dir] = dict()
            quotes[enc_dir][test_dir][SUCCESS] = 0
            quotes[enc_dir][test_dir][FAILURE] = 0

          current_path = join(path, img_dir, 'nist', key_dir, enc_dir, test_dir)
          results[enc_dir][test_dir].append( read_results(current_path) )

          (s,f) = read_stats(current_path)
          quotes[enc_dir][test_dir][SUCCESS] += int(s)
          quotes[enc_dir][test_dir][FAILURE] += int(f)

    write_results(join(path, 'nist_results.csv'), results)
    write_quotes(join(path, 'nist_results_quotes.csv'), quotes)
  
  print("DONE.")

# ----------------------------------------------------------------------------------
# Read NIST output
# ----------------------------------------------------------------------------------
def read_results(path):
  r = list()
  with open(join(path, RESLUTS)) as fp:
    for line in fp:
      r.append(float(line))
  return r

def read_stats(path):
  s = 0; f = 0
  if 'LinearComplexity' in path:
    with open(join(path, RESLUTS)) as fp:
      for line in fp:
        if float(line.strip()) > 0.01:
          s += 1
        else:
          f += 1
  else:
    with open(join(path, STATS)) as fp:
      for line in fp:
        if SUCCESS in line:
          s += 1
        elif FAILURE in line:
          f += 1
  return (s, f)

# ----------------------------------------------------------------------------------
# Write output
# ----------------------------------------------------------------------------------
def write_results(output, results):
  with open(output, 'w+') as results_out:
    writer = csv.writer(results_out)
    for enc in sorted(results):
      for test in sorted(results[enc]):
        row = (enc, test, ) + tuple(results[enc][test])
        writer.writerow(row)

def write_quotes(output, quotes):
  with open(output, 'w+') as quotes_out:
    writer = csv.writer(quotes_out)
    writer.writerow(('encryption', 'test', 'success', 'total', 'quote'))
    
    for enc in sorted(quotes):
      for test in sorted(quotes[enc]):
        total = quotes[enc][test][SUCCESS] + quotes[enc][test][FAILURE]
        if total == 0:
          quote = 0
        else:
          quote = quotes[enc][test][SUCCESS] / total

        row = (enc, test, quotes[enc][test][SUCCESS], total, quote)
        writer.writerow(row)

# ----------------------------------------------------------------------------------
# Utilities
# ----------------------------------------------------------------------------------
def get_dirs(path):
  return [d for d in listdir(path) if isdir(join(path, d))]

def get_arith_mean(numbers):
  if len(numbers) < 1:
    return 0
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
    return 0

  ss = float(0)
  for number in numbers:
    ss = ss + math.pow((float(number) - arith_mean), 2)

  return math.sqrt( (1.0/float(len(numbers)-1)) * ss )

# ----------------------------------------------------------------------------------
# Startup script
# ----------------------------------------------------------------------------------
if __name__=='__main__':
  print ("\n\tusage: python prepare_nist_data.py path/to/nist_data")
  print ("\tusage: expected folder-structure: out/image_name_xy/nist/nist_test_xy\n")

  if len(sys.argv) == 2:
    prepare(sys.argv[1])
