OCAMLCFLAGS = -thread -I ../../lib/ocaml
PACKAGES = -package result,threads,oUnit,thrift
RESULT = ocaml_nicejson

GENOCAML_test = \
	gen-ocaml/test_types.ml \
	gen-ocaml/test_consts.ml \
	gen-ocaml/test_types.mli

BUILT_SOURCES = $(GENOCAML_test)
BUILT_OBJECTS =
	gen-ocaml/test_types.cmo \
	gen-ocaml/test_consts.cmo \
	gen-ocaml/test_types.cmi

check-local: all-local
	./nicejson_tests.byte

all-local: nicejson_tests.byte

nicejson_tests.byte: $(BUILT_OBJECTS) nicejson_tests.cmo
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o nicejson_tests.byte ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml $(BUILT_OBJECTS) nicejson_tests.cmo -cclib -lstdc++

clean-local:
	rm -rf oUnit* *.cm* *.byte

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift

$(GENOCAML_test):
	$(THRIFT) --gen ocaml -r ../cpp/test.thrift


.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<
