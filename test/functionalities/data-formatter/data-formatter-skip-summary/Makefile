LEVEL = ../../../make

CXX_SOURCES := main.cpp

# clang-3.5+ outputs FullDebugInfo by default for Darwin/FreeBSD 
# targets.  Other targets do not, which causes this test to fail.
# This flag enables FullDebugInfo for all targets.
ifneq (,$(findstring clang,$(CC)))
  CFLAGS_EXTRAS := -fstandalone-debug
endif

include $(LEVEL)/Makefile.rules

CXXFLAGS += -O0

ifeq (,$(findstring gcc,$(CC)))
CXXFLAGS += -stdlib=libstdc++
LDFLAGS += -stdlib=libstdc++
endif
