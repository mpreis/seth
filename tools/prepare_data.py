# Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is
# governed  by a BSD license that can be found in the LICENSE file.

import argparse
import csv
import math
import collections
from operator import itemgetter

CSV_COL_ENC = 0
CSV_COL_TEST = 1
CSV_COL_DATA_START = 2

def prepare_data(i, o):
  outdict = collections.defaultdict(dict)
  with open(i) as inputcsv:
    reader = csv.reader(inputcsv, delimiter=';', quotechar='\'')
    for row in reader:
      enc = row[CSV_COL_ENC]
      test = row[CSV_COL_TEST]
      data = row[CSV_COL_DATA_START:len(row)]
      avg = get_arith_mean(data)
      ssd = get_std_dev(data)
      label_test_avg = test + '_avg'
      label_test_ssd = test + '_ssd'

      encdict = outdict[enc]
      encdict[label_test_avg] = avg
      encdict[label_test_ssd] = ssd
      outdict[enc] = encdict 

  outlist = []
  enckeys = outdict.keys()
  for enckey in enckeys:
    tmpdict = outdict[enckey].copy()
    tmpdict['_encryption'] = enckey 
    outlist.append( tmpdict )

  outlistsorted = sorted(outlist, key=itemgetter('_encryption'), reverse=False)
  with open(o, 'w') as outputcsv:
    fieldnames = outlistsorted[0].keys()
    writer = csv.DictWriter(outputcsv, fieldnames=sorted(fieldnames))
    
    writer.writeheader()
    for row in outlistsorted:
      writer.writerow(row)
      
# ----------------------------------------------------------
# MATH HELPER
# ----------------------------------------------------------     
def get_arith_mean(numbers):
  if len(numbers) < 1:
    return 0;
  s = float(0)
  for number in numbers:
    s = s + float(number)
  return s / len(numbers)

def roundup(x, upTo):
  return int(math.ceil(x / float(upTo))) * int(upTo)

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

# ----------------------------------------------------------
# MAIN
# ----------------------------------------------------------
if __name__=='__main__':
  parser = argparse.ArgumentParser(description='')
  parser.add_argument('-i', nargs='?', required=True, 
    help='i is the input file (required format: CSV).')
  parser.add_argument('-o', nargs='?', required=True, 
    help='o is the output file.')
  args = parser.parse_args()
  prepare_data(args.i, args.o)

  