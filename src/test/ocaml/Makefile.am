OCAMLCFLAGS = -thread -I ../../lib/ocaml -I . -I ./gen-ocaml
PACKAGES = -package result,threads,oUnit,thrift,yojson
RESULT = ocaml_nicejson

GENOCAML_test = \
	gen-ocaml/test_types.ml \
	gen-ocaml/test_consts.ml \
	gen-ocaml/test_types.mli

BUILT_SOURCES = $(GENOCAML_test)
BUILT_OBJECTS = \
	gen-ocaml/test_types.cmo \
	gen-ocaml/test_consts.cmo

check-local: all-local
	./nicejson_tests.byte

all-local: nicejson_tests.byte

nicejson_tests.byte: $(BUILT_OBJECTS) TBytesTransport.cmo SerDes.cmo nicejson_tests.cmo
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o nicejson_tests.byte ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml $^ -cclib -lstdc++

clean-local:
	rm -rf oUnit* *.cm* *.byte

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
PLUGIN_THRIFT=$(THRIFT_SRC_ROOT)/compiler/cpp/src/thrift/plugin/plugin.thrift

$(GENOCAML_test):
	$(THRIFT) --gen ocaml -r ../cpp/test.thrift

.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

gen-ocaml/test_types.cmi: gen-ocaml/test_types.mli
gen-ocaml/test_types.cmx gen-ocaml/test_types.cmo: gen-ocaml/test_types.cmi
nicejson_tests.cmo: TBytesTransport.cmo SerDes.cmo
SerDes.cmo SerDes.cmx: TBytesTransport.cmo TBytesTransport.cmi
TBytesTransport.cmi: TBytesTransport.ml
