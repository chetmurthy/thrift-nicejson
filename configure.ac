#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIR([./aclocal])
AC_INIT([thrift-nicejson], [0.01], [chetsky@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects tar-ustar])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_LANG([C++])
m4_include([aclocal/ax_cxx_compile_stdcxx.m4])
AX_CXX_COMPILE_STDCXX(11)


#AX_CXX_COMPILE_STDCXX([11], [noext], [optional])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES([thrift], [thrift >= 0.10.0])
AC_SUBST([thrift_LIBDIR], [$(pkg-config  --variable=libdir --print-errors "thrift >= 0.10.0")])


AX_BOOST_BASE([1.53.0],, [AC_MSG_ERROR([libnicejson needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AC_SUBST([BOOST_LIB_DIR], [$(echo "$BOOST_LDFLAGS" | sed -e 's/^\-L//')])
AC_SUBST([BOOST_FILESYSTEM_LDADD], [$(echo "-lboost_filesystem")])
AC_SUBST([BOOST_SYSTEM_LDADD], [$(echo "-lboost_system")])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/lib/Makefile
                 src/plugins/Makefile
                 src/test/Makefile])
AC_OUTPUT
