AUTOMAKE_OPTIONS = foreign
LIB_SUBDIRS = src/lib/cpp
PLUGIN_SUBDIRS = src/plugins/cpp
TEST_SUBDIRS = src/test/cpp
EXAMPLES_SUBDIR = src/examples/cpp
TOOLS_SUBDIR = src/tools/cpp

if WITH_PYTHON
LIB_SUBDIRS +=  src/lib/py
TEST_SUBDIRS +=  src/test/py
endif

if WITH_OCAML
LIB_SUBDIRS +=  src/lib/ocaml
PLUGIN_SUBDIRS +=  src/plugins/ocaml
TEST_SUBDIRS +=  src/test/ocaml
endif

SUBDIRS = $(LIB_SUBDIRS) $(PLUGIN_SUBDIRS) $(TEST_SUBDIRS) $(EXAMPLES_SUBDIRS) $(TOOLS_SUBDIRS)
