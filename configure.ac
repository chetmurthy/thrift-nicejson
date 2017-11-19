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
LT_INIT

#AX_CXX_COMPILE_STDCXX([11], [noext], [optional])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([thrift], [thrift >= 0.10.0])
AC_SUBST([thrift_LIBDIR], [$(pkg-config  --variable=libdir --print-errors "thrift >= 0.10.0")])
AC_SUBST([thrift_EXEC_PREFIX], [$(pkg-config  --variable=exec_prefix --print-errors "thrift >= 0.10.0")])
AC_SUBST([thrift_INCLUDEDIR], [$(pkg-config  --variable=includedir --print-errors "thrift >= 0.10.0")])


AX_BOOST_BASE([1.53.0],, [AC_MSG_ERROR([libnicejson needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS
AC_SUBST([BOOST_LIB_DIR], [$(echo "$BOOST_LDFLAGS" | sed -e 's/^\-L//')])
AC_SUBST([BOOST_FILESYSTEM_LDADD], [$(echo "-lboost_filesystem")])
AC_SUBST([BOOST_SYSTEM_LDADD], [$(echo "-lboost_system")])

AC_ARG_WITH([thrift-srcdir],
  [AS_HELP_STRING([--with-thrift-srcdir],
    [Directory to the (required) out-of-tree Thrift source])],,
    [AC_MSG_FAILURE([MUST specify out-of-tree Thrift source directory])])
AC_SUBST([THRIFT_SRC_ROOT],[$withval])


AC_ARG_VAR([PY_PREFIX], [Prefix for installing Python modules.
                         (Normal --prefix is ignored for Python because
                         Python has different conventions.)
                         Default = "/usr"])
AS_IF([test "x$PY_PREFIX" = x], [PY_PREFIX="/usr"])
# Find python regardless of with_python value, because it's needed by make cross
AM_PATH_PYTHON(2.6,, :)
AX_THRIFT_LIB(python, [Python], yes)
if test "$with_python" = "yes";  then
  if test -n "$PYTHON"; then
    have_python="yes"
  fi
fi
AM_CONDITIONAL(WITH_PYTHON, [test "$have_python" = "yes"])

AC_PROG_OCAML
AC_PROG_FINDLIB
AC_CHECK_OCAML_PKG([result])
AC_CHECK_OCAML_PKG([oUnit])
AC_CHECK_OCAML_PKG([yojson])
AC_CHECK_OCAML_PKG([thrift])
AC_CHECK_OCAML_PKG([cppffigen])

if test "$OCAMLC" != "no"; then
if test "$OCAMLFIND" = "no" ; then
  AC_MSG_ERROR([Please install OCaml findlib (the ocamlfind command)])
fi

if test "$OCAML_PKG_result" = "no" ; then
  AC_MSG_ERROR([Please install OCaml package 'result'.])
fi

if test "$OCAML_PKG_oUnit" = "no" ; then
  AC_MSG_ERROR([Please install OCaml package 'oUnit'.])
fi

if test "$OCAML_PKG_cppffigen" = "no" ; then
  AC_MSG_ERROR([Please install OCaml package 'cppffigen'.])
fi

fi


AX_THRIFT_LIB(ocaml, [Ocaml], yes)
if test "$with_ocaml" = "yes";  then
  if test "$OCAMLC" != "no"; then
    have_ocaml="yes"
  fi
fi
AM_CONDITIONAL(WITH_OCAML, [test "$have_ocaml" = "yes"])


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
                 src/lib/cpp/Makefile
                 src/lib/py/Makefile
                 src/lib/ocaml/Makefile
                 src/plugins/cpp/Makefile
                 src/test/cpp/Makefile
                 src/test/py/Makefile
                 src/test/ocaml/Makefile
		 src/tools/cpp/Makefile])
AC_OUTPUT
