LEVEL = ../../make

CXX_SOURCES := main.cpp

# Clean renamed executable on 'make clean'
clean: OBJECTS+=no_synth

# clang-3.5+ outputs FullDebugInfo by default for Darwin/FreeBSD 
# targets.  Other targets do not, which causes this test to fail.
# This flag enables FullDebugInfo for all targets.
ifneq (,$(findstring clang,$(CC)))
  CFLAGS_EXTRAS := -fstandalone-debug
endif

include $(LEVEL)/Makefile.rules
