GTEST_DIR = /usr/src/gtest
AM_CPPFLAGS = -isystem $(GTEST_DIR)/include
AM_CXXFLAGS = -g -Wall -Wextra -pthread
GTESTHEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

BUILT_SOURCES = gtest-all.cc gtest_main.cc
CLEANFILES = $(BUILT_SOURCES)

lib_LIBRARIES=libgtest.a libgtestmain.a

libgtest_a_SOURCES= gtest-all.cc
libgtest_a_CXXFLAGS = -fPIC $(AM_CPPFLAGS) $(AM_CXXFLAGS) -I$(GTEST_DIR)

libgtestmain_a_SOURCES= gtest_main.cc
libgtestmain_a_CXXFLAGS = -fPIC $(AM_CPPFLAGS) $(AM_CXXFLAGS) -I$(GTEST_DIR)

gtest-all.cc: $(GTEST_DIR)/src/gtest-all.cc
	cp $< $@

gtest_main.cc: $(GTEST_DIR)/src/gtest_main.cc
	cp $< $@
