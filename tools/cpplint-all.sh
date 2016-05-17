#!/bin/bash

# Copyright (c) 2016,  the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is
# governed  by a BSD license that can be found in the LICENSE file.

SRC="./src"
SRC_SETH_H="$SRC/seth.h"
SRC_SETH_CPP="$SRC/seth.cpp"
SRC_ENCRY="$SRC/encryption"
SRC_TESTS="$SRC/tests"
CMD="python tools/cpplint.py --filter=-build/header_guard"

if [ -z "$1" ]; 
  then
    echo "Argument required:"
    echo "  -a ... all ($SRC)"
    echo "  -e ... encryptions ($SRC_ENCRY)"
    echo "  -t ... tests ($SRC_TESTS)"
  else
    if [ $1 == "-a" ] || [ $1 == "-e" ] ; then
      find $SRC_ENCRY -type f -name *.h -exec $CMD \{\} \;
      find $SRC_ENCRY -type f -name *.cpp -exec $CMD \{\} \;
    fi

    if [ $1 == "-a" ] || [ $1 == "-t" ] ; then
      find $SRC_TESTS -type f -name *.h -exec $CMD \{\} \;
      find $SRC_TESTS -type f -name *.cpp -exec $CMD \{\} \;
    fi

    if [ $1 == "-a" ] || [ $1 == "-s" ] ; then
      $CMD $SRC_SETH_H
      $CMD $SRC_SETH_CPP
    fi
fi