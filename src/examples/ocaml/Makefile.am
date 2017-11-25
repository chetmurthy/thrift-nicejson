OCAMLCFLAGS = -thread -I ../../lib/ocaml -I . -I ./gen-ocaml
PACKAGES = -package result,threads,oUnit,thrift,yojson
RESULT = ocaml_nicejson

GENOCAML_tutorial = \
	gen-ocaml/Calculator.ml \
	gen-ocaml/Calculator.mli \
	gen-ocaml/SharedService.ml \
	gen-ocaml/SharedService.mli \
	gen-ocaml/shared_consts.ml \
	gen-ocaml/shared_types.ml \
	gen-ocaml/shared_types.mli \
	gen-ocaml/tutorial_consts.ml \
	gen-ocaml/tutorial_types.ml \
	gen-ocaml/tutorial_types.mli

GENTYPELIB_tutorial = \
	gen-typelib/shared.shared.binary_typelib \
	gen-typelib/shared.shared.typelib \
	gen-typelib/tutorial.tutorial.binary_typelib \
	gen-typelib/tutorial.tutorial.typelib

BUILT_SOURCES = $(GENOCAML_tutorial) $(GENTYPELIB_tutorial)
BUILT_OBJECTS = \
	gen-ocaml/shared_consts.cmo \
	gen-ocaml/shared_types.cmo \
	gen-ocaml/SharedService.cmo \
	gen-ocaml/tutorial_consts.cmo \
	gen-ocaml/tutorial_types.cmo \
	gen-ocaml/Calculator.cmo

all-local: CalcServer.byte CalcClient.byte

CalcServer.byte: $(BUILT_OBJECTS) CalcServer.cmo
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o $@ ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml $^ -cclib -lstdc++

CalcClient.byte: $(BUILT_OBJECTS) CalcClient.cmo
	$(OCAMLFIND) ocamlc -custom $(OCAMLCFLAGS) $(PACKAGES) -linkpkg -linkall -o $@ ../../lib/ocaml/$(RESULT).cma  -cclib -L../../lib/ocaml $^ -cclib -lstdc++

clean-local:
	rm -rf oUnit* *.cm* *.byte gen-ocaml/*.cm*

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
TUTORIAL_THRIFT=$(THRIFT_SRC_ROOT)/tutorial/tutorial.thrift

$(GENOCAML_tutorial): $(TUTORIAL_THRIFT)
	$(THRIFT) --gen ocaml -r $^

$(GENTYPELIB_tutorial): $(TUTORIAL_THRIFT)
	PATH=../../plugins/cpp:${PATH} $(THRIFT) -r -gen typelib:typelib $^

.ml.cmo:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.ml.cmx:
	$(OCAMLFIND) ocamlopt $(OCAMLCFLAGS) $(PACKAGES) -c $<

.mli.cmi:
	$(OCAMLFIND) ocamlc $(OCAMLCFLAGS) $(PACKAGES) -c $<

.NOTPARALLEL:

gen-ocaml/Calculator.cmo: gen-ocaml/tutorial_types.cmi gen-ocaml/SharedService.cmi gen-ocaml/Calculator.cmi
gen-ocaml/SharedService.cmi: gen-ocaml/shared_types.cmi
gen-ocaml/SharedService.cmo: gen-ocaml/SharedService.cmi
gen-ocaml/tutorial_consts.cmo: gen-ocaml/tutorial_types.cmi
