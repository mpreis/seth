# Copyright (c) 2016, the Seth Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is
# governed  by a BSD license that can be found in the LICENSE file.

PROG = seth
SRCS = ./tests/correlation_property_test/cpt.cpp \
	./tests/entropy/entropy.cpp \
	./tests/npcr/npcr.cpp \
	./tests/uaci/uaci.cpp \
	./tests/sequence_test/sequence_test.cpp \
	./tests/key_sensitivity_test/key_sensitivity_test.cpp \
	./tests/grey_scale_value/grey_scale_value.cpp \
	./tests/grey_scale_histogram/grey_scale_histogram.cpp \
	./tests/nist_test_suite/nist.cpp \
	./encryption/arnolds_cat_map/arnolds_cat_map.cpp \
	./encryption/bakers_map/bakers_map.cpp \
	./encryption/xor_otp/xor_otp_cstd.cpp \
	./encryption/xor_otp/xor_otp_rc4.cpp \
	./encryption/xor_short_key/xor_short_key.cpp \
	./encryption/xor_followers/xor_followers.cpp \
	./encryption/aes/aes.cpp \
	./encryption/2d_logistic_map/2d_logistic_map.cpp

BUILDDIR = ../build
OBJSDIR = $(BUILDDIR)/objs
NISTOBJS = $(OBJSDIR)/approximateEntropy.o \
	$(OBJSDIR)/blockFrequency.o \
	$(OBJSDIR)/cephes.o \
	$(OBJSDIR)/cusum.o \
	$(OBJSDIR)/dfft.o \
	$(OBJSDIR)/discreteFourierTransform.o \
	$(OBJSDIR)/frequency.o \
	$(OBJSDIR)/generators.o \
	$(OBJSDIR)/genutils.o \
	$(OBJSDIR)/linearComplexity.o \
	$(OBJSDIR)/longestRunOfOnes.o \
	$(OBJSDIR)/matrix.o \
	$(OBJSDIR)/nonOverlappingTemplateMatchings.o \
	$(OBJSDIR)/overlappingTemplateMatchings.o \
	$(OBJSDIR)/randomExcursions.o \
	$(OBJSDIR)/randomExcursionsVariant.o \
	$(OBJSDIR)/rank.o \
	$(OBJSDIR)/runs.o \
	$(OBJSDIR)/serial.o \
	$(OBJSDIR)/universal.o \
	$(OBJSDIR)/utilities.o

CXXFLAGS=-std=c++0x

all: seth

seth:
	g++ $(CXXFLAGS) -o $(PROG) $(PROG).cpp $(SRCS) $(NISTOBJS) -lm -lpthread -I/usr/X11R6/include -lcryptopp -Llibraries/cryptopp562
	mv $(PROG) $(BUILDDIR)

clean:
	rm -rf *o

fullclean: clean
	find ./src -type f -name '*.o' -delete
