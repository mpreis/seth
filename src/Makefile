PROG = seth
SRCS = ./tests/correlation_property_test/cpt.cpp \
		./tests/entropy/entropy.cpp \
		./tests/npcr/npcr.cpp \
		./tests/uaci/uaci.cpp \
		./tests/sequence_test/sequence_test.cpp \
		./tests/key_sensitivity_test/key_sensitivity_test.cpp \
		./tests/grey_scale_value/grey_scale_value.cpp \
 		./tests/grey_scale_histogram/grey_scale_histogram.cpp \
		./encryption/arnolds_cat_map/arnolds_cat_map.cpp \
		./encryption/bakers_map/bakers_map.cpp \
		./encryption/xor_otp/xor_otp_cstd.cpp \
		./encryption/xor_otp/xor_otp_rc4.cpp \
		./encryption/xor_short_key/xor_short_key.cpp \
		./encryption/xor_followers/xor_followers.cpp
BUILDDIR = ../build/
CXXFLAGS=-std=c++11 
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
all: mac

server: $(PROG).o $(OBJS) 
	g++ $(CXXFLAGS) -o $(PROG) $(PROG).cpp $(SRCS) -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11 
	mv $(PROG) $(BUILDDIR)

mac: $(PROG).o $(OBJS)
	g++ $(CXXFLAGS) -o $(PROG) $(PROG).cpp $(SRCS) -lm -lpthread -I/usr/X11R6/include 
	mv $(PROG) $(BUILDDIR)

clean:
	rm -rf *o

fullclean: clean
	find ./src -type f -name '*.o' -delete
