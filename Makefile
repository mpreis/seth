# Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is
# governed  by a BSD license that can be found in the LICENSE file.

all: mkdirs cryptopp nist
	cd ./src; make
	
clean:
	rm -rf build

fullclean: clean
	find . -type f -name '*.o' -delete

cryptopp:
	cd ./src/libraries/cryptopp562/; make static
	cd ../../../

nist:
	cd ./src/tests/nist_test_suite/; make nist_objs
	cd ../../../

mkdirs:
	mkdir -p build
	mkdir -p build/objs
