OCAMLCFLAGS = -thread -I ../../lib/ocaml -I . -I ./gen-ocaml
PACKAGES = -package result,threads,oUnit,thrift,yojson
RESULT = ocaml_nicejson

bin_PROGRAMS = thrift-gen-ocamltypelib

GENOCAML_plugin = \
	gen-ocaml/plugin_types.ml \
	gen-ocaml/plugin_consts.ml \
	gen-ocaml/plugin_types.mli

BUILT_SOURCES = $(GENOCAML_plugin) .depend
BUILT_OBJECTS = \
	gen-ocaml/plugin_types.cmo \
	gen-ocaml/plugin_consts.cmo

all-local: thrift-gen-ocamltypelib

thrift-gen-ocamltypelib: $(BUILT_OBJECTS) thrift-gen-ocamltypelib.cmo
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o $@ ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml $^ -cclib -lstdc++

clean-local:
	rm -rf *.cm* *.byte gen-ocaml/*.cm*

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
PLUGIN_THRIFT=$(THRIFT_SRC_ROOT)/compiler/cpp/src/thrift/plugin/plugin.thrift

$(GENOCAML_plugin): $(PLUGIN_THRIFT)
	$(THRIFT) --gen ocaml -r $(PLUGIN_THRIFT)

.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.NOTPARALLEL:

.depend: *.ml* gen-ocaml/*.ml*
	ocamldep $^ > .depend.NEW && mv .depend.NEW .depend
-include .depend
